#include "UI/HTTP/HTTPRequestTypes.h"

#include "MMONetManager/MMONetManager.h"

namespace HTTPStatusMessages
{
	const FString SomethingWentWrongSignUp = TEXT("Something went wrong. User Registration failed.");
	const FString SomethingWentWrongSignIn = TEXT("Something went wrong. Unable to login.");
}

void FNMMetaData::Dump()
{
	UE_LOG(LogMMONetManager, Log, TEXT("MetaData:"));
	UE_LOG(LogMMONetManager, Log, TEXT("	httpStatusCode: %d"), httpStatusCode);
	UE_LOG(LogMMONetManager, Log, TEXT("	requestId: %s"), *requestId);
	UE_LOG(LogMMONetManager, Log, TEXT("	attempts: %d"), attempts);
	UE_LOG(LogMMONetManager, Log, TEXT("	totalRetryDelay: %f"), totalRetryDelay);
}

void FNMCodeDeliveryDetails::Dump() const
{
	UE_LOG(LogMMONetManager, Log, TEXT("CodeDeliveryDetails:"));
	UE_LOG(LogMMONetManager, Log, TEXT("   AttributeName: %s"), *AttributeName);
	UE_LOG(LogMMONetManager, Log, TEXT("   DeliveryMedium: %s"), *DeliveryMedium);
	UE_LOG(LogMMONetManager, Log, TEXT("   Destination: %s"), *Destination);
}

void FNMSignUpResponse::Dump() const
{
	UE_LOG(LogMMONetManager, Log, TEXT("FDSSignUpResponse Details:"));
	CodeDeliveryDetails.Dump();
	UE_LOG(LogMMONetManager, Log, TEXT("	UserConfirmed: %s"), UserConfirmed ? TEXT("true") : TEXT("false"));
	UE_LOG(LogMMONetManager, Log, TEXT("	UserSub: %s"), *UserSub);
}

void FNMNewDeviceMetaData::Dump() const
{
	UE_LOG(LogMMONetManager, Log, TEXT("NewDeviceMetadata:"));
	UE_LOG(LogMMONetManager, Log, TEXT("   DeviceGroupKey: %s"), *DeviceGroupKey);
	UE_LOG(LogMMONetManager, Log, TEXT("   DeviceKey: %s"), *DeviceKey);
}

void FNMAuthenticationResult::Dump() const
{
	UE_LOG(LogMMONetManager, Log, TEXT("AuthenticationResult:"));
	UE_LOG(LogMMONetManager, Log, TEXT("   AccessToken: %s"), *AccessToken);
	UE_LOG(LogMMONetManager, Log, TEXT("   ExpiresIn: %d"), ExpiresIn);
	UE_LOG(LogMMONetManager, Log, TEXT("   IdToken: %s"), *IdToken);
	NewDeviceMetadata.Dump();
	UE_LOG(LogMMONetManager, Log, TEXT("   RefreshToken: %s"), *RefreshToken);
	UE_LOG(LogMMONetManager, Log, TEXT("   TokenType: %s"), *TokenType);
}

void FNMChallengeParameters::Dump() const
{
	UE_LOG(LogMMONetManager, Log, TEXT("ChallengeParameters:"));
	for (const auto& Param : Parameters)
	{
		UE_LOG(LogMMONetManager, Log, TEXT("   %s: %s"), *Param.Key, *Param.Value);
	}
}

void FNMInitiateAuthResponse::Dump() const
{
	UE_LOG(LogMMONetManager, Log, TEXT("FDSSignUpResponse Details:"));
	AuthenticationResult.Dump();
	UE_LOG(LogMMONetManager, Log, TEXT("ChallengeName: %s"), *ChallengeName);
	ChallengeParameters.Dump();
	UE_LOG(LogMMONetManager, Log, TEXT("Session: %s"), *Session);
	UE_LOG(LogMMONetManager, Log, TEXT("email: %s"), *Email);
}
