// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AccessPortal/AccessPortalManager.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpResponse.h"
#include "Data/API/APIData.h"
#include "GameplayTags/MMONetManagerTags.h"
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
			LocalPlayerSubsystem->InitializeTokens(InitiateAuthResponse.AuthenticationResult);
			LocalPlayerSubsystem->Username = LastUsername;
			LocalPlayerSubsystem->Email = InitiateAuthResponse.Email;
		}
	}
}