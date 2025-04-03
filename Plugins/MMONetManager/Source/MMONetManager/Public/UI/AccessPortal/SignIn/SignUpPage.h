// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignUpPage.generated.h"

class UEditableTextBox;
class UAccessPortalManager;
class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class MMONETMANAGER_API USignUpPage : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_UserName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_Password;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_ConfirmPassword;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_Email;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_SignUp;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Back;

	/* 
	 * ~Status Messages~
	 */

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StatusMessageValidEmail;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StatusMessageValidUsername;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StatusMessagePasswordHasNumber;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StatusMessagePasswordLongEnough;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StatusMessagePasswordSpecialChar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StatusMessagePasswordUppercase;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StatusMessagePasswordLowercase;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StatusMessagePasswordsMatch;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccessPortalManager> AccessPortalManagerClass;

	void ClearTextBoxes();

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Status Message")
	FSlateColor StatusMessageErrorColor;
	
	UPROPERTY(EditDefaultsOnly, Category = "Status Message")
	FSlateColor StatusMessageSuccessColor;

	virtual void NativeConstruct() override;

private:
	
	bool bIsValidUsername{false};
	bool bIsUniqueUsername{false};
	bool bIsValidEmail{false};
	bool bIsUniqueEmail{false};
	bool bIsStrongPassword{false};
	bool bArePasswordsEqual{false};

	UPROPERTY()
	TObjectPtr<UAccessPortalManager> AccessPortalManager;
	
	UFUNCTION()
	void UpdateSignUpButtonState();
	
	UFUNCTION()
	void UpdateStatusMessageUsername(const FText& Text);
	
	UFUNCTION()
	void OnUsernameCommitted(const FText& Text, ETextCommit::Type CommitType);
	
	UFUNCTION()
	void OnCheckUsernameUnique(bool bIsUnique);

	UFUNCTION()
	void UpdateStatusMessageEmail(const FText& Text);
	
	UFUNCTION()
	void OnEmailCommitted(const FText& Text, ETextCommit::Type CommitType);
	
	UFUNCTION()
	void OnCheckEmailUnique(bool bIsUnique);

	UFUNCTION()
	void UpdateStatusMessagePassword(const FText& Text);
	
	UFUNCTION()
	void UpdateStatusMessageConfirmPassword(const FText& Text);
	
	bool IsValidEmail(const FString& Email);
	
	bool IsStrongPassword(const FString& Password);
};


