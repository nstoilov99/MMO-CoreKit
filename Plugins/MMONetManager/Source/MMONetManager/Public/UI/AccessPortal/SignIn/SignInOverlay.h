// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignInOverlay.generated.h"

class UAccessPortalManager;
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

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccessPortalManager> AccessPortalManagerClass;
	
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

	UPROPERTY()
	TObjectPtr<UAccessPortalManager> AccessPortalManager;

	UFUNCTION()
	void SignUpButtonClicked();
	
	UFUNCTION()
	void SignInButtonClicked();

	UFUNCTION()
	void ShowSignInPage();

	UFUNCTION()
	void ShowSignUpPage();
};
