// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatusLogMessageRow.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class MMONETMANAGER_API UStatusLogMessageRow : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetStatusLogMessage(const FString& StatusLogMessage);
	
protected:
	UPROPERTY(BlueprintReadWrite, Category = "StatusLog", meta = (BindWidget))
	TObjectPtr<UTextBlock> StatusLogMessage;
};
