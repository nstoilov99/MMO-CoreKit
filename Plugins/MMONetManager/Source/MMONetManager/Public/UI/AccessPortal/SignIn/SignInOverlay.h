// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignInOverlay.generated.h"

class USignUpPage;
class USignInPage;
class UWidgetSwitcher;
/**
 * 
 */
UCLASS()
class MMONETMANAGER_API USignInOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;
	
protected:
	virtual void NativeConstruct() override;

private:

	//TODO: Steam Login later
	/*UPROPERTY(meta = (BindWidget))
	TObjectPtr<USignInPageSteam> SignInPageSteam;*/

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USignInPage> SignInPage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USignUpPage> SignUpPage;
};
