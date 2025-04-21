// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayFabClientAPI.h"
#include "Interfaces/AccessPortalManagement.h"
#include "Interfaces/IHttpRequest.h"
#include "UI/HTTP/HTTPRequestManager.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "AccessPortalManager.generated.h"

/**
 * 
 */
UCLASS()
class MMONETMANAGER_API UAccessPortalManager : public UHTTPRequestManager, public IAccessPortalManagement
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FAPIStatusLogMessage SignUpStatusMessageDelegate;

	UPROPERTY(BlueprintAssignable)
	FAPIErrorMessagePopup SignUpErrorMessagePopupDelegate;

	UPROPERTY(BlueprintAssignable)
	FAPIStatusLogMessage SignInStatusMessageDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnAPIRequestSucceeded OnSignUpSucceeded;
	
	UPROPERTY(BlueprintAssignable)
	FOnAPIRequestSucceeded OnSignInSucceeded;
	
	FString LastUsername;
	FNMSignUpResponse LastSignUpResponse;

	void SignUp(const FString& Username, const FString& Password, const FString& Email);
	void PlayFabSignUp(const FString& Username, const FString& Password, const FString& Email);

	UFUNCTION()
	void OnRegisterSuccessCallback(FClientRegisterPlayFabUserResult Result, UObject* CustomData);
	
	UFUNCTION()
	void OnRegisterErrorCallback(FPlayFabError Error, UObject* CustomData);

	void SignIn(const FString& Username, const FString& Password);

	// IAccessPortalManagement
	virtual void RefreshTokens(const FString& RefreshToken) override;
private:
	
	UPROPERTY()
	TObjectPtr<UPlayFabClientAPI> PlayFabClientAPI;

	FString ContactEmail{};

	void SignUp_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void SignIn_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void RefreshTokens_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
