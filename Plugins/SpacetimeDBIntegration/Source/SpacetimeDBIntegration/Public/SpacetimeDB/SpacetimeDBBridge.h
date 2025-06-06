// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SpacetimeDBBridge.generated.h"

extern "C" {
	struct FFIResult;
    
	FFIResult spacetimedb_connect(const char* host, uint16_t port, const char* database_name);
	FFIResult spacetimedb_register_user(const char* username, const char* password, const char* email);
	FFIResult spacetimedb_login_user(const char* username, const char* password);
	FFIResult spacetimedb_join_game(const char* starting_zone);
	FFIResult spacetimedb_update_position(float x, float y, float z, float yaw);
	FFIResult spacetimedb_send_chat(const char* message, const char* channel);
	void spacetimedb_free_result(FFIResult* result);
	void spacetimedb_free_string(char* ptr);
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConnectionResult, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAuthResult, bool, bSuccess, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerJoined, const FString&, Username, const FVector&, Position);

/**
 * 
*/
UCLASS()
class SPACETIMEDBINTEGRATION_API USpacetimeDBBridge : public UObject
{
	GENERATED_BODY()

public:
	USpacetimeDBBridge();

	// Connection Management
	UFUNCTION(BlueprintCallable, Category = "SpacetimeDB")
	bool ConnectToServer(const FString& Host, int32 Port, const FString& DatabaseName);

	// Authentication
	UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|Auth")
	bool RegisterUser(const FString& Username, const FString& Password, const FString& Email = "");

	UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|Auth")
	bool LoginUser(const FString& Username, const FString& Password);

	// Game World
	UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|Game")
	bool JoinGame(const FString& StartingZone);

	UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|Game")
	bool UpdatePlayerPosition(const FVector& Position, float Yaw);

	// Chat
	UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|Chat")
	bool SendChatMessage(const FString& Message, const FString& Channel);

	// Events
	UPROPERTY(BlueprintAssignable, Category = "SpacetimeDB|Events")
	FOnConnectionResult OnConnectionResult;

	UPROPERTY(BlueprintAssignable, Category = "SpacetimeDB|Events")
	FOnAuthResult OnAuthResult;

	UPROPERTY(BlueprintAssignable, Category = "SpacetimeDB|Events")
	FOnPlayerJoined OnPlayerJoined;

private:
	bool bIsConnected;
	FString LastErrorMessage;

	// Helper functions
	FString ProcessFFIResult(const FFIResult& Result);
	void BroadcastAuthResult(bool bSuccess, const FString& ErrorMessage);
};
