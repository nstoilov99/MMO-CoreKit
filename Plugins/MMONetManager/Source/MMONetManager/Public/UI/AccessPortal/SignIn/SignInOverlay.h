// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignInOverlay.generated.h"

class UStatusLogWidget;
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

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USignInPage> SignInPage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USignUpPage> SignUpPage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStatusLogWidget> StatusLog;

	UPROPERTY()
	TObjectPtr<UAccessPortalManager> AccessPortalManager;

	UFUNCTION()
	void SignUpButtonClicked();

	UFUNCTION()
	void SignUpStatusMessage(const FString& StatusMessage);
	
	UFUNCTION()
	void SignInButtonClicked();

	UFUNCTION()
	void ShowSignInPage();

	UFUNCTION()
	void ShowSignUpPage();
};
