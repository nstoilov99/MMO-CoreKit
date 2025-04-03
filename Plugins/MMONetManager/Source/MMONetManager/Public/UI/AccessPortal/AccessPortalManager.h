// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	FAPIStatusMessage SignUpStatusMessageDelegate;


	UPROPERTY(BlueprintAssignable)
	FAPIStatusMessage SignInStatusMessageDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnAPIRequestSucceeded OnSignUpSucceeded;
	
	UPROPERTY(BlueprintAssignable)
	FOnAPIRequestSucceeded OnSignInSucceeded;
	
	UPROPERTY(BlueprintAssignable)
	FOnFieldChecked OnFieldCheckedUniqueUsername;
	
	UPROPERTY(BlueprintAssignable)
	FOnFieldChecked OnFieldCheckedUniqueEmail;
	
	FString LastUsername;
	FNMSignUpResponse LastSignUpResponse;

	void SignUp(const FString& Username, const FString& Password, const FString& Email);

	void SignIn(const FString& Username, const FString& Password);

	void UniqueEmail(const FString& Email);

	void UniqueUsername(const FString& Username);
	// IAccessPortalManagement
	virtual void RefreshTokens(const FString& RefreshToken) override;
private:

	void SignUp_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void SignIn_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void RefreshTokens_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void UniqueEmail_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void UniqueUsername_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
