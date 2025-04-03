// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AccessPortal/AccessPortalManager.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpResponse.h"
#include "Data/API/APIData.h"
#include "GameplayTags/MMONetManagerTags.h"
#include "MMONetManager/MMONetManager.h"
#include "Player/NetManagerLocalPlayerSubsystem.h"
#include "UI/HTTP/HTTPRequestTypes.h"

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

void UAccessPortalManager::SignUp_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		SignUpStatusMessageDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrongSignUp, true);
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))
		{
			SignUpStatusMessageDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrongSignUp, true);
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
			SignInStatusMessageDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrongSignUp, true);
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


void UAccessPortalManager::UniqueEmail(const FString& Email)
{
	check(APIData);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UAccessPortalManager::UniqueEmail_Response);
	const FString APIUrl = APIData->GetAPIEndpoint(MMONetManagerTags::AccessPortalAPI::UniqueUsername_Email);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TMap<FString, FString> Params = {
		{ TEXT("email"), Email }
	};
	const FString Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	Request->ProcessRequest();
}

void UAccessPortalManager::UniqueEmail_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogMMONetManager, Error, TEXT("UNIQUE Email: HTTP Request Failed!"))
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))
		{
			UE_LOG(LogMMONetManager, Error, TEXT("JSON Response contains errors"));
			return;
		}
		bool bEmailExists = JsonObject->GetBoolField(TEXT("emailExists"));
		OnFieldCheckedUniqueEmail.Broadcast(!bEmailExists);
	}
}

void UAccessPortalManager::UniqueUsername(const FString& Username)
{
	check(APIData);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UAccessPortalManager::UniqueUsername_Response);
	const FString APIUrl = APIData->GetAPIEndpoint(MMONetManagerTags::AccessPortalAPI::UniqueUsername_Email);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TMap<FString, FString> Params = {
		{ TEXT("username"), Username }
	};
	const FString Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	Request->ProcessRequest();
}


void UAccessPortalManager::UniqueUsername_Response(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogMMONetManager, Error, TEXT("UNIQUE Username: HTTP Request Failed!"))
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))
		{
			UE_LOG(LogMMONetManager, Error, TEXT("JSON Response contains errors"));
			return;
		}
		bool bUsernameExists = JsonObject->GetBoolField(TEXT("usernameExists"));
		OnFieldCheckedUniqueUsername.Broadcast(!bUsernameExists);
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
