// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NetManagerLocalPlayerSubsystem.h"

#include "Interfaces/AccessPortalManagement.h"

void UNetManagerLocalPlayerSubsystem::InitializeTokens(const FNMAuthenticationResult& AuthResult, TScriptInterface<IAccessPortalManagement> AccessPortalManagement)
{
	AuthenticationResult = AuthResult;
	AccessPortalManagementInterface = AccessPortalManagement;
	SetRefreshTokenTimer();
}

void UNetManagerLocalPlayerSubsystem::SetRefreshTokenTimer()
{
	UWorld* World = GetWorld();
	if (IsValid(World) && IsValid(AccessPortalManagementInterface.GetObject()))
	{
		FTimerDelegate RefreshDelegate;
		RefreshDelegate.BindLambda([this](){ AccessPortalManagementInterface->RefreshTokens(AuthenticationResult.RefreshToken); });
		World->GetTimerManager().SetTimer(RefreshTimer, RefreshDelegate, TokenRefreshInterval, false);
	}
}

void UNetManagerLocalPlayerSubsystem::UpdateTokens(const FString& AccessToken, const FString& IdToken)
{
	AuthenticationResult.AccessToken = AccessToken;
	AuthenticationResult.IdToken = IdToken;
	AuthenticationResult.Dump();
	SetRefreshTokenTimer();
}

FNMAuthenticationResult UNetManagerLocalPlayerSubsystem::GetAuthenticationResult() const
{
	return AuthenticationResult;
}
