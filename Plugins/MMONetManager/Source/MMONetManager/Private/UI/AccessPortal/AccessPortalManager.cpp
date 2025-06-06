// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AccessPortal/AccessPortalManager.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "PlayFabClientAPI.h"
#include "Interfaces/IHttpResponse.h"
#include "Data/API/APIData.h"
#include "GameplayTags/MMONetManagerTags.h"
#include "Player/NetManagerLocalPlayerSubsystem.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "PlayFabClientModels.h"

void UAccessPortalManager::SignUp(const FString& Username, const FString& Password, const FString& Email)
{
	check(APIData);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UAccessPortalManager::SignUp_Response);
	const FString APIUrl = APIData->GetAPIEndpoint(MMONetManagerTags::AccessPortalAPI::SignUp);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	LastUsername = Username;
	TMap<FString, FString> Params = {
		{ TEXT("username"), Username },
		{ TEXT("password"), Password },
		{ TEXT("email"), Email }
	};
	const FString Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	Request->ProcessRequest();
}

void UAccessPortalManager::PlayFabSignUp(const FString& Username, const FString& Password, const FString& Email)
{
	SignUpStatusMessageDelegate.Broadcast("Registering...");
	FClientRegisterPlayFabUserRequest Request = FClientRegisterPlayFabUserRequest();
	Request.Email = Email;
	ContactEmail = Email;
	Request.Password = Password;
	Request.Username = Username;
	Request.DisplayName = Username;
	Request.RequireBothUsernameAndEmail = true;
			
	UPlayFabClientAPI::FDelegateOnSuccessRegisterPlayFabUser SuccessDelegate;
	UPlayFabClientAPI::FDelegateOnFailurePlayFabError FailureDelegate;

	SuccessDelegate.BindDynamic(this, &UAccessPortalManager::OnRegisterSuccessCallback);
	FailureDelegate.BindDynamic(this, &UAccessPortalManager::OnRegisterErrorCallback);
	

	PlayFabClientAPI = UPlayFabClientAPI::RegisterPlayFabUser(
		Request,
		SuccessDelegate,
		FailureDelegate,
		this
	);
	if (IsValid(PlayFabClientAPI))
	{
		SignUpStatusMessageDelegate.Broadcast("Sending a request...");
		PlayFabClientAPI->Activate();
	}
	else
	{
		SignUpErrorMessagePopupDelegate.Broadcast("Failed to send a request!", true);
	}
}

void UAccessPortalManager::OnRegisterSuccessCallback(FClientRegisterPlayFabUserResult Result, UObject* CustomData)
{
	SignUpStatusMessageDelegate.Broadcast("Successful registration!");
	OnSignUpSucceeded.Broadcast();
	FClientAddOrUpdateContactEmailRequest ContactEmailRequest;

	ContactEmailRequest.EmailAddress = ContactEmail;

	UPlayFabClientAPI::FDelegateOnSuccessAddOrUpdateContactEmail UpdateContactEmailSuccessDelegate;
	UPlayFabClientAPI::FDelegateOnFailurePlayFabError UpdateContactEmailFailureDelegate;

	UpdateContactEmailSuccessDelegate.BindDynamic(this, &UAccessPortalManager::OnAddOrUpdateContactEmailSucceeded);
	UpdateContactEmailFailureDelegate.BindDynamic(this, &UAccessPortalManager::OnAddOrUpdateContactEmailFailed);

	PlayFabClientAPI = UPlayFabClientAPI::AddOrUpdateContactEmail(
		ContactEmailRequest,
		UpdateContactEmailSuccessDelegate,
		UpdateContactEmailFailureDelegate,
		this
		);

	if (IsValid(PlayFabClientAPI))
	{
		PlayFabClientAPI->Activate();
	}
}

void UAccessPortalManager::OnRegisterErrorCallback(FPlayFabError Error, UObject* CustomData)
{
	UE_LOG(LogTemp, Error, TEXT("❌ Registration failed: %s"), *Error.ErrorMessage);
}

void UAccessPortalManager::OnAddOrUpdateContactEmailSucceeded(FClientAddOrUpdateContactEmailResult Result,
	UObject* CustomData)
{
	OnUpdateContactEmailSucceeded.Broadcast();
}

void UAccessPortalManager::OnAddOrUpdateContactEmailFailed(FPlayFabError Error, UObject* CustomData)
{
	
}

void UAccessPortalManager::SignUp_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))
		{
			return;
		}
		
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &LastSignUpResponse);
		LastSignUpResponse.Dump();
		OnSignUpSucceeded.Broadcast();
	}
}

void UAccessPortalManager::SignIn(const FString& Username, const FString& Password)
{
	check(APIData);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UAccessPortalManager::SignIn_Response);
	const FString APIUrl = APIData->GetAPIEndpoint(MMONetManagerTags::AccessPortalAPI::SignIn);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	LastUsername = Username;
	TMap<FString, FString> Params = {
		{ TEXT("username"), Username },
		{ TEXT("password"), Password }
	};
	const FString Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	Request->ProcessRequest();
}

void UAccessPortalManager::SignIn_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))
		{
			return;
		}

		FNMInitiateAuthResponse InitiateAuthResponse;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &InitiateAuthResponse);
		InitiateAuthResponse.Dump();

		OnSignInSucceeded.Broadcast();
		
		UNetManagerLocalPlayerSubsystem* LocalPlayerSubsystem = GetNMLocalPlayerSubsystem();
		if (IsValid(LocalPlayerSubsystem))
		{
			LocalPlayerSubsystem->InitializeTokens(InitiateAuthResponse.AuthenticationResult, this);
			LocalPlayerSubsystem->Username = LastUsername;
			LocalPlayerSubsystem->Email = InitiateAuthResponse.Email;
		}
	}
}

void UAccessPortalManager::RefreshTokens(const FString& RefreshToken)
{
	check(APIData);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UAccessPortalManager::RefreshTokens_Response);
	const FString APIUrl = APIData->GetAPIEndpoint(MMONetManagerTags::AccessPortalAPI::SignIn);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	TMap<FString, FString> Params = {
		{ TEXT("refreshToken"), RefreshToken }
	};
	const FString Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	Request->ProcessRequest();
}

void UAccessPortalManager::RefreshTokens_Response(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bWasSuccessful)
{
	if (!bWasSuccessful) return;

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject)) return;

		FNMInitiateAuthResponse InitiateAuthResponse;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &InitiateAuthResponse);

		UNetManagerLocalPlayerSubsystem* LocalPlayerSubsystem = GetNMLocalPlayerSubsystem();
		if (IsValid(LocalPlayerSubsystem))
		{
			LocalPlayerSubsystem->UpdateTokens(
				InitiateAuthResponse.AuthenticationResult.AccessToken,
				InitiateAuthResponse.AuthenticationResult.IdToken
				);
		}
	}
}
