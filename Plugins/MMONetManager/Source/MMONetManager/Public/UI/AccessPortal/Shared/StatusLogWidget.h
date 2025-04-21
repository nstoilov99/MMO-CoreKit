// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatusLogWidget.generated.h"

class UStatusLogMessageRow;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class MMONETMANAGER_API UStatusLogWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void AddStatusMessage(const FString& Message);

protected:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> StatusLogBox;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UStatusLogMessageRow> MessageRowClass;

private:
	UPROPERTY()
	TArray<UUserWidget*> ActiveMessages;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	int32 MaxMessages;
};
