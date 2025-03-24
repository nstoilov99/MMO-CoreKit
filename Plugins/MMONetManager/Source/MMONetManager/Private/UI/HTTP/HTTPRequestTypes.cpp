#include "UI/HTTP/HTTPRequestTypes.h"

#include "MMONetManager/MMONetManager.h"

namespace HTTPStatusMessages
{
	const FString SomethingWentWrongSignUp = TEXT("Something went wrong. User Registration failed.");
}

void FNMMetaData::Dump()
{
	UE_LOG(LogMMONetManager, Log, TEXT("MetaData:"));
	UE_LOG(LogMMONetManager, Log, TEXT("	httpStatusCode: %d"), httpStatusCode);
	UE_LOG(LogMMONetManager, Log, TEXT("	requestId: %s"), *requestId);
	UE_LOG(LogMMONetManager, Log, TEXT("	attempts: %d"), attempts);
	UE_LOG(LogMMONetManager, Log, TEXT("	totalRetryDelay: %f"), totalRetryDelay);
}

void FDSCodeDeliveryDetails::Dump() const
{
	UE_LOG(LogMMONetManager, Log, TEXT("CodeDeliveryDetails:"));
	UE_LOG(LogMMONetManager, Log, TEXT("   AttributeName: %s"), *AttributeName);
	UE_LOG(LogMMONetManager, Log, TEXT("   DeliveryMedium: %s"), *DeliveryMedium);
	UE_LOG(LogMMONetManager, Log, TEXT("   Destination: %s"), *Destination);
}

void FDSSignUpResponse::Dump() const
{
	UE_LOG(LogMMONetManager, Log, TEXT("FDSSignUpResponse Details:"));
	CodeDeliveryDetails.Dump();
	UE_LOG(LogMMONetManager, Log, TEXT("	UserConfirmed: %s"), UserConfirmed ? TEXT("true") : TEXT("false"));
	UE_LOG(LogMMONetManager, Log, TEXT("	UserSub: %s"), *UserSub);
}
