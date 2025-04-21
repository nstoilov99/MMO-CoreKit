// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NetManagerInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MMONETMANAGER_API UNetManagerInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UNetManagerInstanceSubsystem();

	UFUNCTION(BlueprintCallable)
	FString GetSteamUsername() const;

	UPROPERTY(BlueprintReadOnly)
	FString Username;
	
private:

	FUniqueNetIdPtr SteamId;
	
	IOnlineIdentityPtr IdentityInterface;
};
