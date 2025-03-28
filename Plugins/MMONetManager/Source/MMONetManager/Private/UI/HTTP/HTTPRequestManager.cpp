// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HTTP/HTTPRequestManager.h"

#include "JsonObjectConverter.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "Player/NetManagerLocalPlayerSubsystem.h"
#include "MMONetManager/MMONetManager.h"


UNetManagerLocalPlayerSubsystem* UHTTPRequestManager::GetNMLocalPlayerSubsystem() const
{
	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(LocalPlayerController))
	{
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(LocalPlayerController->GetLocalPlayer());
		if (IsValid(LocalPlayer))
		{
			return LocalPlayer->GetSubsystem<UNetManagerLocalPlayerSubsystem>();
		}
	}
	return nullptr;
}

// TODO: Make better error handling 
bool UHTTPRequestManager::ContainsErrors(TSharedPtr<FJsonObject> JsonObject)
{
	if (JsonObject->HasField(TEXT("errorType")) || JsonObject->HasField(TEXT("errorMessage")))
	{
		FString ErrorType = JsonObject->HasField(TEXT("errorType")) ? JsonObject->GetStringField(TEXT("errorType")) : TEXT("Unknown Error");
		FString ErrorMessage = JsonObject->HasField(TEXT("errorMessage")) ? JsonObject->GetStringField(TEXT("errorMessage")) : TEXT("Unknown Error Message");

		UE_LOG(LogMMONetManager, Error, TEXT("Error Type: %s"), *ErrorType);
		UE_LOG(LogMMONetManager, Error, TEXT("Error Message: %s"), *ErrorMessage);

		return true;
	}
	if (JsonObject->HasField(TEXT("$fault")))
	{
		FString ErrorType = JsonObject->HasField(TEXT("name")) ? JsonObject->GetStringField(TEXT("name")) : TEXT("Unknown Error");
		UE_LOG(LogMMONetManager, Error, TEXT("Error Type: %s"), *ErrorType);
		return true;
	}
	return false;
}

void UHTTPRequestManager::DumpMetaData(TSharedPtr<FJsonObject> JsonObject)
{
	if (JsonObject->HasField(TEXT("$metadata")))
	{
		TSharedPtr<FJsonObject> MetaDataJsonObject = JsonObject->GetObjectField(TEXT("$metadata"));
		FNMMetaData NMMetaData;
		FJsonObjectConverter::JsonObjectToUStruct(MetaDataJsonObject.ToSharedRef(), &NMMetaData);
		NMMetaData.Dump();
	}
}

FString UHTTPRequestManager::SerializeJsonContent(const TMap<FString, FString>& Params)
{
	TSharedPtr<FJsonObject> ContentJsonObject = MakeShareable(new FJsonObject);

	for (const auto& Param : Params)
	{
		ContentJsonObject->SetStringField(Param.Key, Param.Value);
	}

	FString Content;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(ContentJsonObject.ToSharedRef(), JsonWriter);
	
	return Content;
}
