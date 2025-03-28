#pragma once

#include "HTTPRequestTypes.generated.h"

namespace HTTPStatusMessages
{
	extern MMONETMANAGER_API const FString SomethingWentWrongSignUp;
}

USTRUCT()
struct FNMMetaData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 httpStatusCode{};

	UPROPERTY()
	FString requestId{};

	UPROPERTY()
	int32 attempts{};

	UPROPERTY()
	double totalRetryDelay{};

	void Dump();
};

USTRUCT()
struct FNMCodeDeliveryDetails
{
	GENERATED_BODY()

	UPROPERTY()
	FString AttributeName{};

	UPROPERTY()
	FString DeliveryMedium{};

	UPROPERTY()
	FString Destination{};

	void Dump() const;
};

USTRUCT()
struct FNMSignUpResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FNMCodeDeliveryDetails CodeDeliveryDetails{};

	UPROPERTY()
	bool UserConfirmed{};

	UPROPERTY()
	FString UserSub{};

	void Dump() const;
};

USTRUCT()
struct FNMNewDeviceMetaData
{
	GENERATED_BODY()

	UPROPERTY()
	FString DeviceGroupKey{};

	UPROPERTY()
	FString DeviceKey{};

	void Dump() const;
};

USTRUCT()
struct FNMAuthenticationResult
{
	GENERATED_BODY()

	UPROPERTY()
	FString AccessToken{};

	UPROPERTY()
	int32 ExpiresIn{};

	UPROPERTY()
	FString IdToken{};

	UPROPERTY()
	FNMNewDeviceMetaData NewDeviceMetadata{};

	UPROPERTY()
	FString RefreshToken{};

	UPROPERTY()
	FString TokenType{};

	void Dump() const;
};

// ChallengeParameters struct
USTRUCT()
struct FNMChallengeParameters
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FString, FString> Parameters{};

	void Dump() const;
};

USTRUCT()
struct FNMInitiateAuthResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FNMAuthenticationResult AuthenticationResult{};

	UPROPERTY()
	FString ChallengeName{};

	UPROPERTY()
	FNMChallengeParameters ChallengeParameters{};

	UPROPERTY()
	FString Session{};

	UPROPERTY()
	FString Email{};

	void Dump() const;
};