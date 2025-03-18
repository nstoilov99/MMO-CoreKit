// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignInPageSteam.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class MMONETMANAGER_API USignInPageSteam : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SignInButton;
};
