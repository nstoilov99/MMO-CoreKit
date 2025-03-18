#include "UI/HTTP/HTTPRequestTypes.h"

#include "MMONetManager/MMONetManager.h"


void FNMMetaData::Dump()
{
	UE_LOG(LogMMONetManager, Log, TEXT("MetaData:"));
	UE_LOG(LogMMONetManager, Log, TEXT("	httpStatusCode: %d"), httpStatusCode);
	UE_LOG(LogMMONetManager, Log, TEXT("	requestId: %s"), *requestId);
	UE_LOG(LogMMONetManager, Log, TEXT("	attempts: %d"), attempts);
	UE_LOG(LogMMONetManager, Log, TEXT("	totalRetryDelay: %f"), totalRetryDelay);
}
