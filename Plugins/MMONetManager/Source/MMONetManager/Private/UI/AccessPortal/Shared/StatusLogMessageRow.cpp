// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AccessPortal/Shared/StatusLogMessageRow.h"

#include "Components/TextBlock.h"

void UStatusLogMessageRow::SetStatusLogMessage(const FString& LogMessage)
{
	StatusLogMessage->SetText(FText::FromString(LogMessage));
}
