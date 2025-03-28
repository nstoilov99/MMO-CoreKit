// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "NetManagerLocalPlayerSubsystem.generated.h"

class IAccessPortalManagement;
/**
 * 
 */
UCLASS()
class MMONETMANAGER_API UNetManagerLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	void InitializeTokens(const FNMAuthenticationResult& AuthenticationResult, TScriptInterface<IAccessPortalManagement> AccessPortalManagement);
	void SetRefreshTokenTimer();
	void UpdateTokens(const FString& AccessToken, const FString& IdToken);

	FNMAuthenticationResult GetAuthenticationResult() const;
	
	FString Username{};
	FString Email{};
	FString Password{};
	

private:

	UPROPERTY()
	FNMAuthenticationResult AuthenticationResult;
	
	UPROPERTY()
	TScriptInterface<IAccessPortalManagement> AccessPortalManagementInterface;

	// 75% of an hour (the expiration time for AccessToken and IdToken)
	float TokenRefreshInterval = 2700.f;
	FTimerHandle RefreshTimer;
};
