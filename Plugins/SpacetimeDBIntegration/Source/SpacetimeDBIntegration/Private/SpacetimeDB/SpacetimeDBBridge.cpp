// Fill out your copyright notice in the Description page of Project Settings.


#include "SpacetimeDB/SpacetimeDBBridge.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Async/Async.h"
#include "SpacetimeDB/SpacetimeDBTypes.h"

USpacetimeDBBridge::USpacetimeDBBridge()
{
    bIsConnected = false;
    LastErrorMessage = "";
}

bool USpacetimeDBBridge::ConnectToServer(const FString& Host, int32 Port, const FString& DatabaseName)
{
    // Convert FString to C strings
    const char* HostCStr = TCHAR_TO_ANSI(*Host);
    const char* DatabaseCStr = TCHAR_TO_ANSI(*DatabaseName);
    
    // Call the FFI function
    FFIResult Result = spacetimedb_connect(HostCStr, static_cast<uint16_t>(Port), DatabaseCStr);
    
    // Process the result
    FString ErrorMessage = ProcessFFIResult(Result);
    bIsConnected = Result.success;
    
    // Broadcast result to blueprints
    AsyncTask(ENamedThreads::GameThread, [this, bSuccess = Result.success]()
    {
        OnConnectionResult.Broadcast(bSuccess);
    });
    
    // Free the result
    spacetimedb_free_result(&Result);
    
    return Result.success;
}

bool USpacetimeDBBridge::RegisterUser(const FString& Username, const FString& Password, const FString& Email)
{
    if (!bIsConnected)
    {
        BroadcastAuthResult(false, TEXT("Not connected to server"));
        return false;
    }
    
    // Convert strings
    const char* UsernameCStr = TCHAR_TO_ANSI(*Username);
    const char* PasswordCStr = TCHAR_TO_ANSI(*Password);
    const char* EmailCStr = Email.IsEmpty() ? nullptr : TCHAR_TO_ANSI(*Email);
    
    // Call FFI function
    FFIResult Result = spacetimedb_register_user(UsernameCStr, PasswordCStr, EmailCStr);
    
    // Process result
    FString ErrorMessage = ProcessFFIResult(Result);
    
    // Broadcast to game thread
    AsyncTask(ENamedThreads::GameThread, [this, bSuccess = Result.success, ErrorMessage]()
    {
        BroadcastAuthResult(bSuccess, ErrorMessage);
    });
    
    spacetimedb_free_result(&Result);
    return Result.success;
}

bool USpacetimeDBBridge::LoginUser(const FString& Username, const FString& Password)
{
    if (!bIsConnected)
    {
        BroadcastAuthResult(false, TEXT("Not connected to server"));
        return false;
    }
    
    const char* UsernameCStr = TCHAR_TO_ANSI(*Username);
    const char* PasswordCStr = TCHAR_TO_ANSI(*Password);
    
    FFIResult Result = spacetimedb_login_user(UsernameCStr, PasswordCStr);
    
    FString ErrorMessage = ProcessFFIResult(Result);
    
    AsyncTask(ENamedThreads::GameThread, [this, bSuccess = Result.success, ErrorMessage]()
    {
        BroadcastAuthResult(bSuccess, ErrorMessage);
    });
    
    spacetimedb_free_result(&Result);
    return Result.success;
}

bool USpacetimeDBBridge::JoinGame(const FString& StartingZone)
{
    if (!bIsConnected)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot join game: Not connected to server"));
        return false;
    }
    
    const char* ZoneCStr = TCHAR_TO_ANSI(*StartingZone);
    FFIResult Result = spacetimedb_join_game(ZoneCStr);
    
    bool bSuccess = Result.success;
    if (bSuccess)
    {
        // Broadcast player joined event
        AsyncTask(ENamedThreads::GameThread, [this, StartingZone]()
        {
            OnPlayerJoined.Broadcast(TEXT("LocalPlayer"), FVector::ZeroVector);
        });
    }
    
    spacetimedb_free_result(&Result);
    return bSuccess;
}

bool USpacetimeDBBridge::UpdatePlayerPosition(const FVector& Position, float Yaw)
{
    if (!bIsConnected)
    {
        return false;
    }
    
    FFIResult Result = spacetimedb_update_position(Position.X, Position.Y, Position.Z, Yaw);
    bool bSuccess = Result.success;
    
    spacetimedb_free_result(&Result);
    return bSuccess;
}

bool USpacetimeDBBridge::SendChatMessage(const FString& Message, const FString& Channel)
{
    if (!bIsConnected)
    {
        return false;
    }
    
    const char* MessageCStr = TCHAR_TO_ANSI(*Message);
    const char* ChannelCStr = TCHAR_TO_ANSI(*Channel);
    
    FFIResult Result = spacetimedb_send_chat(MessageCStr, ChannelCStr);
    bool bSuccess = Result.success;
    
    spacetimedb_free_result(&Result);
    return bSuccess;
}

FString USpacetimeDBBridge::ProcessFFIResult(const FFIResult& Result)
{
    if (!Result.success && Result.error_message)
    {
        FString ErrorMessage = FString(ANSI_TO_TCHAR(Result.error_message));
        LastErrorMessage = ErrorMessage;
        UE_LOG(LogTemp, Error, TEXT("SpacetimeDB Error: %s"), *ErrorMessage);
        return ErrorMessage;
    }
    
    return TEXT("");
}

void USpacetimeDBBridge::BroadcastAuthResult(bool bSuccess, const FString& ErrorMessage)
{
    OnAuthResult.Broadcast(bSuccess, ErrorMessage);
}