// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpacetimeDBTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SpacetimeDBSubsystem.generated.h"


// Delegate declarations for Blueprint events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpacetimeDBConnected, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpacetimeDBError, const FString&, ErrorMessage, int32, ErrorCode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserAuthenticated, bool, bSuccess, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserRegistered, bool, bSuccess, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoinedGame, const FSpacetimeDBPlayer&, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLeft, int64, PlayerId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChatMessageReceived, const FSpacetimeDBChatMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerPositionUpdated, const FSpacetimeDBPlayer&, Player);

/**
 * Main subsystem for SpacetimeDB integration
 * This manages the connection, authentication, and real-time synchronization
 */
UCLASS()
class SPACETIMEDBINTEGRATION_API USpacetimeDBSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	public:
    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Core connection management
    UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|Connection")
    void ConnectToServer(const FString& Host = TEXT("localhost"), int32 Port = 3000, const FString& DatabaseName = TEXT("mmo_database"));

    UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|Connection")
    void Disconnect();

    // Authentication functions
    UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|Auth")
    void RegisterUser(const FString& Username, const FString& Password, const FString& Email = "");

    UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|Auth")
    void LoginUser(const FString& Username, const FString& Password);

    UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|Auth")
    void LogoutUser();

    // Game state management
    UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|Game")
    void JoinGame(const FString& StartingZone = TEXT("default"));

    UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|Game")
    void LeaveGame();

    UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|Game")
    void UpdatePlayerPosition(const FVector& NewPosition, float NewRotation);

    // Chat system
    UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|Chat")
    void SendChatMessage(const FString& Message, const FString& Channel = TEXT("global"));

    // Data queries
    UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|Data")
    TArray<FSpacetimeDBPlayer> GetPlayersInZone(const FString& Zone);

    UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|Data")
    TArray<FSpacetimeDBChatMessage> GetRecentMessages(const FString& Channel, int32 Limit = 20);

    // State getters
    UFUNCTION(BlueprintPure, Category = "SpacetimeDB|State")
    ESpacetimeDBConnectionState GetConnectionState() const { return ConnectionState; }

    UFUNCTION(BlueprintPure, Category = "SpacetimeDB|State")
    bool IsConnected() const { return ConnectionState == ESpacetimeDBConnectionState::Connected || ConnectionState == ESpacetimeDBConnectionState::Authenticated || ConnectionState == ESpacetimeDBConnectionState::InGame; }

    UFUNCTION(BlueprintPure, Category = "SpacetimeDB|State")
    bool IsAuthenticated() const { return ConnectionState == ESpacetimeDBConnectionState::Authenticated || ConnectionState == ESpacetimeDBConnectionState::InGame; }

    UFUNCTION(BlueprintPure, Category = "SpacetimeDB|State")
    bool IsInGame() const { return ConnectionState == ESpacetimeDBConnectionState::InGame; }

    UFUNCTION(BlueprintPure, Category = "SpacetimeDB|State")
    FString GetCurrentUsername() const { return CurrentUsername; }

    // Event delegates that Blueprints can bind to
    UPROPERTY(BlueprintAssignable)
    FOnSpacetimeDBConnected OnConnected;

    UPROPERTY(BlueprintAssignable)
    FOnSpacetimeDBError OnError;

    UPROPERTY(BlueprintAssignable)
    FOnUserAuthenticated OnUserAuthenticated;

    UPROPERTY(BlueprintAssignable)
    FOnUserRegistered OnUserRegistered;

    UPROPERTY(BlueprintAssignable)
    FOnPlayerJoinedGame OnPlayerJoinedGame;

    UPROPERTY(BlueprintAssignable)
    FOnPlayerLeft OnPlayerLeft;

    UPROPERTY(BlueprintAssignable)
    FOnChatMessageReceived OnChatMessageReceived;

    UPROPERTY(BlueprintAssignable)
    FOnPlayerPositionUpdated OnPlayerPositionUpdated;

protected:
    // Internal state
    UPROPERTY()
    ESpacetimeDBConnectionState ConnectionState;

    UPROPERTY()
    FString CurrentUsername;

    UPROPERTY()
    FString ServerHost;

    UPROPERTY()
    int32 ServerPort;

    UPROPERTY()
    FString DatabaseName;

    // Timers for periodic operations
    FTimerHandle PositionUpdateTimer;
    FTimerHandle HeartbeatTimer;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpacetimeDB|Config")
    float PositionUpdateInterval = 0.1f; // Send position updates every 100ms

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpacetimeDB|Config")
    float HeartbeatInterval = 30.0f; // Send heartbeat every 30 seconds

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpacetimeDB|Config")
    float PositionUpdateThreshold = 1.0f; // Only update if moved more than 1 unit

private:
    // Helper functions for FFI result handling
    void HandleFFIResult(const FFIResult& Result, const FString& Operation);
    void SetConnectionState(ESpacetimeDBConnectionState NewState);
    
    // Periodic update functions
    void SendPositionUpdate();
    void SendHeartbeat();
    
    // Last known position for delta checking
    FVector LastSentPosition;
    float LastSentRotation;
};
