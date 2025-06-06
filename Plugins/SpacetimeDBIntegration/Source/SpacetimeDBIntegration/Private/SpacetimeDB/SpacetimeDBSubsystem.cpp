// Fill out your copyright notice in the Description page of Project Settings.


#include "SpacetimeDB/SpacetimeDBSubsystem.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"
#include "Async/Async.h"

void USpacetimeDBSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Initialize state
    ConnectionState = ESpacetimeDBConnectionState::Disconnected;
    CurrentUsername = TEXT("");
    LastSentPosition = FVector::ZeroVector;
    LastSentRotation = 0.0f;
    
    UE_LOG(LogTemp, Log, TEXT("SpacetimeDB Subsystem Initialized"));
}

void USpacetimeDBSubsystem::Deinitialize()
{
    // Clean up any active connections
    if (IsConnected())
    {
        Disconnect();
    }
    
    // Clear timers
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(PositionUpdateTimer);
        World->GetTimerManager().ClearTimer(HeartbeatTimer);
    }
    
    Super::Deinitialize();
}

void USpacetimeDBSubsystem::ConnectToServer(const FString& Host, int32 Port, const FString& DatabaseName)
{
    if (ConnectionState != ESpacetimeDBConnectionState::Disconnected)
    {
        UE_LOG(LogTemp, Warning, TEXT("Already connected or connecting"));
        return;
    }
    
    // Store connection parameters
    ServerHost = Host;
    ServerPort = Port;
    this->DatabaseName = DatabaseName;
    
    SetConnectionState(ESpacetimeDBConnectionState::Connecting);
    
    // Perform connection in background thread to avoid blocking the game thread
    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, Host, Port, DatabaseName]()
    {
        // Convert FString to C strings for FFI
        std::string HostAnsi = TCHAR_TO_UTF8(*Host);
        std::string DbNameAnsi = TCHAR_TO_UTF8(*DatabaseName);
        
        FFIResult Result = spacetimedb_connect(
            HostAnsi.c_str(),
            static_cast<uint16_t>(Port),
            DbNameAnsi.c_str()
        );
        
        // Return to game thread for UI updates
        AsyncTask(ENamedThreads::GameThread, [this, Result]()
        {
            HandleFFIResult(Result, TEXT("Connection"));
            
            if (Result.success)
            {
                SetConnectionState(ESpacetimeDBConnectionState::Connected);
                OnConnected.Broadcast(true);
                
                // Start heartbeat timer
                if (UWorld* World = GetWorld())
                {
                    World->GetTimerManager().SetTimer(
                        HeartbeatTimer,
                        this,
                        &USpacetimeDBSubsystem::SendHeartbeat,
                        HeartbeatInterval,
                        true
                    );
                }
            }
            else
            {
                SetConnectionState(ESpacetimeDBConnectionState::Error);
                FString ErrorMsg = Result.error_message ? 
                    FString(UTF8_TO_TCHAR(Result.error_message)) : 
                    TEXT("Unknown connection error");
                OnError.Broadcast(ErrorMsg, 1001);
                OnConnected.Broadcast(false);
            }
            
            // Always clean up the FFI result
            spacetimedb_free_result(const_cast<FFIResult*>(&Result));
        });
    });
}

void USpacetimeDBSubsystem::Disconnect()
{
    if (ConnectionState == ESpacetimeDBConnectionState::Disconnected)
    {
        return;
    }
    
    // Clear timers
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(PositionUpdateTimer);
        World->GetTimerManager().ClearTimer(HeartbeatTimer);
    }
    
    // In a real implementation, you'd call a disconnect FFI function here
    
    SetConnectionState(ESpacetimeDBConnectionState::Disconnected);
    CurrentUsername = TEXT("");
    
    UE_LOG(LogTemp, Log, TEXT("Disconnected from SpacetimeDB server"));
}

void USpacetimeDBSubsystem::RegisterUser(const FString& Username, const FString& Password, const FString& Email)
{
    if (!IsConnected())
    {
        OnUserRegistered.Broadcast(false, TEXT("Not connected to server"));
        return;
    }
    
    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, Username, Password, Email]()
    {
        std::string UsernameAnsi = TCHAR_TO_UTF8(*Username);
        std::string PasswordAnsi = TCHAR_TO_UTF8(*Password);
        std::string EmailAnsi = Email.IsEmpty() ? "" : TCHAR_TO_UTF8(*Email);
        
        FFIResult Result = spacetimedb_register_user(
            UsernameAnsi.c_str(),
            PasswordAnsi.c_str(),
            Email.IsEmpty() ? nullptr : EmailAnsi.c_str()
        );
        
        AsyncTask(ENamedThreads::GameThread, [this, Result, Username]()
        {
            HandleFFIResult(Result, TEXT("Registration"));
            
            if (Result.success)
            {
                OnUserRegistered.Broadcast(true, TEXT(""));
                UE_LOG(LogTemp, Log, TEXT("User registered successfully: %s"), *Username);
            }
            else
            {
                FString ErrorMsg = Result.error_message ? 
                    FString(UTF8_TO_TCHAR(Result.error_message)) : 
                    TEXT("Registration failed");
                OnUserRegistered.Broadcast(false, ErrorMsg);
            }
            
            spacetimedb_free_result(const_cast<FFIResult*>(&Result));
        });
    });
}

void USpacetimeDBSubsystem::LoginUser(const FString& Username, const FString& Password)
{
    if (!IsConnected())
    {
        OnUserAuthenticated.Broadcast(false, TEXT("Not connected to server"));
        return;
    }
    
    if (ConnectionState != ESpacetimeDBConnectionState::Connected)
    {
        OnUserAuthenticated.Broadcast(false, TEXT("Invalid connection state for login"));
        return;
    }
    
    SetConnectionState(ESpacetimeDBConnectionState::Authenticating);
    
    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, Username, Password]()
    {
        std::string UsernameAnsi = TCHAR_TO_UTF8(*Username);
        std::string PasswordAnsi = TCHAR_TO_UTF8(*Password);
        
        FFIResult Result = spacetimedb_login_user(
            UsernameAnsi.c_str(),
            PasswordAnsi.c_str()
        );
        
        AsyncTask(ENamedThreads::GameThread, [this, Result, Username]()
        {
            HandleFFIResult(Result, TEXT("Login"));
            
            if (Result.success)
            {
                CurrentUsername = Username;
                SetConnectionState(ESpacetimeDBConnectionState::Authenticated);
                OnUserAuthenticated.Broadcast(true, TEXT(""));
                UE_LOG(LogTemp, Log, TEXT("User logged in successfully: %s"), *Username);
            }
            else
            {
                SetConnectionState(ESpacetimeDBConnectionState::Connected);
                FString ErrorMsg = Result.error_message ? 
                    FString(UTF8_TO_TCHAR(Result.error_message)) : 
                    TEXT("Login failed");
                OnUserAuthenticated.Broadcast(false, ErrorMsg);
            }
            
            spacetimedb_free_result(const_cast<FFIResult*>(&Result));
        });
    });
}

void USpacetimeDBSubsystem::JoinGame(const FString& StartingZone)
{
    if (!IsAuthenticated())
    {
        UE_LOG(LogTemp, Warning, TEXT("Must be authenticated to join game"));
        return;
    }
    
    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, StartingZone]()
    {
        std::string ZoneAnsi = TCHAR_TO_UTF8(*StartingZone);
        
        FFIResult Result = spacetimedb_join_game(ZoneAnsi.c_str());
        
        AsyncTask(ENamedThreads::GameThread, [this, Result, StartingZone]()
        {
            HandleFFIResult(Result, TEXT("Join Game"));
            
            if (Result.success)
            {
                SetConnectionState(ESpacetimeDBConnectionState::InGame);
                
                // Create a dummy player for the event (in a real implementation, 
                // this would come from the server)
                FSpacetimeDBPlayer Player;
                Player.Username = CurrentUsername;
                Player.Position = FVector::ZeroVector;
                Player.bIsOnline = true;
                Player.CurrentZone = StartingZone;
                
                OnPlayerJoinedGame.Broadcast(Player);
                
                // Start position update timer
                if (UWorld* World = GetWorld())
                {
                    World->GetTimerManager().SetTimer(
                        PositionUpdateTimer,
                        this,
                        &USpacetimeDBSubsystem::SendPositionUpdate,
                        PositionUpdateInterval,
                        true
                    );
                }
                
                UE_LOG(LogTemp, Log, TEXT("Successfully joined game in zone: %s"), *StartingZone);
            }
            else
            {
                FString ErrorMsg = Result.error_message ? 
                    FString(UTF8_TO_TCHAR(Result.error_message)) : 
                    TEXT("Failed to join game");
                OnError.Broadcast(ErrorMsg, 1003);
            }
            
            spacetimedb_free_result(const_cast<FFIResult*>(&Result));
        });
    });
}

void USpacetimeDBSubsystem::UpdatePlayerPosition(const FVector& NewPosition, float NewRotation)
{
    if (!IsInGame())
    {
        return;
    }
    
    // Only send update if position changed significantly
    float DistanceMoved = FVector::Dist(NewPosition, LastSentPosition);
    float RotationDelta = FMath::Abs(NewRotation - LastSentRotation);
    
    if (DistanceMoved > PositionUpdateThreshold || RotationDelta > 5.0f)
    {
        AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, NewPosition, NewRotation]()
        {
            FFIResult Result = spacetimedb_update_position(
                NewPosition.X,
                NewPosition.Y,
                NewPosition.Z,
                NewRotation
            );
            
            AsyncTask(ENamedThreads::GameThread, [this, Result, NewPosition, NewRotation]()
            {
                if (Result.success)
                {
                    LastSentPosition = NewPosition;
                    LastSentRotation = NewRotation;
                }
                else
                {
                    HandleFFIResult(Result, TEXT("Position Update"));
                }
                
                spacetimedb_free_result(const_cast<FFIResult*>(&Result));
            });
        });
    }
}

void USpacetimeDBSubsystem::SendChatMessage(const FString& Message, const FString& Channel)
{
    if (!IsInGame())
    {
        UE_LOG(LogTemp, Warning, TEXT("Must be in game to send chat messages"));
        return;
    }
    
    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, Message, Channel]()
    {
        std::string MessageAnsi = TCHAR_TO_UTF8(*Message);
        std::string ChannelAnsi = TCHAR_TO_UTF8(*Channel);
        
        FFIResult Result = spacetimedb_send_chat(
            MessageAnsi.c_str(),
            ChannelAnsi.c_str()
        );
        
        AsyncTask(ENamedThreads::GameThread, [this, Result, Message, Channel]()
        {
            if (Result.success)
            {
                // In a real implementation, the message would come back from the server
                // For now, we'll create a local echo
                FSpacetimeDBChatMessage ChatMessage;
                ChatMessage.SenderUsername = CurrentUsername;
                ChatMessage.Message = Message;
                ChatMessage.Channel = Channel;
                ChatMessage.Timestamp = FDateTime::Now();
                
                OnChatMessageReceived.Broadcast(ChatMessage);
                
                UE_LOG(LogTemp, Log, TEXT("Chat message sent to %s: %s"), *Channel, *Message);
            }
            else
            {
                HandleFFIResult(Result, TEXT("Send Chat"));
            }
            
            spacetimedb_free_result(const_cast<FFIResult*>(&Result));
        });
    });
}

void USpacetimeDBSubsystem::SendPositionUpdate()
{
    // Get the current player's position
    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            if (APawn* Pawn = PC->GetPawn())
            {
                FVector CurrentPosition = Pawn->GetActorLocation();
                float CurrentRotation = Pawn->GetActorRotation().Yaw;
                
                UpdatePlayerPosition(CurrentPosition, CurrentRotation);
            }
        }
    }
}

void USpacetimeDBSubsystem::SendHeartbeat()
{
    // In a real implementation, this would send a heartbeat to keep the connection alive
    UE_LOG(LogTemp, VeryVerbose, TEXT("Sending heartbeat to SpacetimeDB server"));
}

void USpacetimeDBSubsystem::HandleFFIResult(const FFIResult& Result, const FString& Operation)
{
    if (!Result.success)
    {
        FString ErrorMsg = Result.error_message ? 
            FString(UTF8_TO_TCHAR(Result.error_message)) : 
            FString::Printf(TEXT("%s failed"), *Operation);
        
        UE_LOG(LogTemp, Error, TEXT("SpacetimeDB Error [%s]: %s"), *Operation, *ErrorMsg);
        OnError.Broadcast(ErrorMsg, 1000);
    }
}

void USpacetimeDBSubsystem::SetConnectionState(ESpacetimeDBConnectionState NewState)
{
    if (ConnectionState != NewState)
    {
        ESpacetimeDBConnectionState OldState = ConnectionState;
        ConnectionState = NewState;
        
        UE_LOG(LogTemp, Log, TEXT("Connection state changed from %d to %d"), 
            static_cast<int32>(OldState), static_cast<int32>(NewState));
    }
}

// Stub implementations for query functions
// In a real implementation, these would make actual calls to SpacetimeDB
TArray<FSpacetimeDBPlayer> USpacetimeDBSubsystem::GetPlayersInZone(const FString& Zone)
{
    TArray<FSpacetimeDBPlayer> Players;
    // This would query the server for players in the specified zone
    return Players;
}

TArray<FSpacetimeDBChatMessage> USpacetimeDBSubsystem::GetRecentMessages(const FString& Channel, int32 Limit)
{
    TArray<FSpacetimeDBChatMessage> Messages;
    // This would query the server for recent chat messages
    return Messages;
}