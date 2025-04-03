// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AccessPortal/SignIn/SignUpPage.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "UI/AccessPortal/AccessPortalManager.h"

void USignUpPage::ClearTextBoxes()
{
	TextBox_Email->SetText(FText::GetEmpty());
	TextBox_UserName->SetText(FText::GetEmpty());
	TextBox_Password->SetText(FText::GetEmpty());
	TextBox_ConfirmPassword->SetText(FText::GetEmpty());
}

void USignUpPage::NativeConstruct()
{
	Super::NativeConstruct();
	AccessPortalManager = NewObject<UAccessPortalManager>(this, AccessPortalManagerClass);

	AccessPortalManager->OnFieldCheckedUniqueEmail.AddDynamic(this, &USignUpPage::OnCheckEmailUnique);
	AccessPortalManager->OnFieldCheckedUniqueUsername.AddDynamic(this, &USignUpPage::OnCheckUsernameUnique);
	
	TextBlock_StatusMessagePasswordHasNumber->SetColorAndOpacity(StatusMessageErrorColor);
	TextBlock_StatusMessagePasswordSpecialChar->SetColorAndOpacity(StatusMessageErrorColor);
	TextBlock_StatusMessagePasswordUppercase->SetColorAndOpacity(StatusMessageErrorColor);
	TextBlock_StatusMessagePasswordLowercase->SetColorAndOpacity(StatusMessageErrorColor);
	
	TextBox_Email->OnTextChanged.AddDynamic(this, &USignUpPage::UpdateStatusMessageEmail);
	TextBox_Email->OnTextCommitted.AddDynamic(this, &USignUpPage::OnEmailCommitted);
	TextBox_UserName->OnTextChanged.AddDynamic(this, &USignUpPage::UpdateStatusMessageUsername);
	TextBox_UserName->OnTextCommitted.AddDynamic(this, &USignUpPage::OnUsernameCommitted);
	TextBox_Password->OnTextChanged.AddDynamic(this, &USignUpPage::UpdateStatusMessagePassword);
	TextBox_ConfirmPassword->OnTextChanged.AddDynamic(this, &USignUpPage::UpdateStatusMessageConfirmPassword);
	Button_SignUp->SetIsEnabled(false);
}

void USignUpPage::UpdateSignUpButtonState()
{
	Button_SignUp->SetIsEnabled(bIsValidUsername && bIsUniqueUsername && bArePasswordsEqual && bIsValidEmail && bIsUniqueEmail && bIsStrongPassword);
}

void USignUpPage::UpdateStatusMessageUsername(const FText& Text)
{
	bIsValidUsername = TextBox_UserName->GetText().ToString().Len() >= 4;
	if (bIsValidUsername)
	{
		TextBlock_StatusMessageValidUsername->SetVisibility(ESlateVisibility::Collapsed);

		//TODO: check if username already exists
	}
	else
	{
		TextBlock_StatusMessageValidUsername->SetVisibility(ESlateVisibility::Visible);
		TextBlock_StatusMessageValidUsername->SetText(FText::FromString(TEXT("Username must be 4 characters long.")));
	}
	UpdateSignUpButtonState();
}

void USignUpPage::OnUsernameCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	if (CommitType != ETextCommit::OnCleared && bIsValidUsername)
	{
		AccessPortalManager->UniqueUsername(TextBox_UserName->GetText().ToString());
	}
}

void USignUpPage::OnCheckUsernameUnique(bool bIsUnique)
{
	bIsUniqueUsername = bIsUnique;
	if (bIsUniqueUsername)
	{
		TextBlock_StatusMessageValidUsername->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		TextBlock_StatusMessageValidUsername->SetVisibility(ESlateVisibility::Visible);
		TextBlock_StatusMessageValidUsername->SetText(FText::FromString(TEXT("Username is already in use.")));
	}
	UpdateSignUpButtonState();
}

void USignUpPage::UpdateStatusMessageEmail(const FText& Text)
{
	bIsValidEmail = IsValidEmail(TextBox_Email->GetText().ToString());
	if (bIsValidEmail)
	{
		TextBlock_StatusMessageValidEmail->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		TextBlock_StatusMessageValidEmail->SetVisibility(ESlateVisibility::Visible);
		TextBlock_StatusMessageValidEmail->SetText(FText::FromString(TEXT("Please enter a valid email.")));
	}
	UpdateSignUpButtonState();
}

void USignUpPage::OnEmailCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	if (CommitType != ETextCommit::OnCleared && bIsValidEmail)
	{
		AccessPortalManager->UniqueEmail(TextBox_Email->GetText().ToString());
	}
}

void USignUpPage::OnCheckEmailUnique(bool bIsUnique)
{
	bIsUniqueEmail = bIsUnique;
	if (bIsUniqueEmail)
	{
		TextBlock_StatusMessageValidEmail->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		TextBlock_StatusMessageValidEmail->SetVisibility(ESlateVisibility::Visible);
		TextBlock_StatusMessageValidEmail->SetText(FText::FromString(TEXT("Email is already in use.")));
	}
	UpdateSignUpButtonState();
}

void USignUpPage::UpdateStatusMessagePassword(const FText& Text)
{
	const bool bIsPasswordLongEnough = TextBox_Password->GetText().ToString().Len() >= 8;
	bIsStrongPassword = IsStrongPassword(TextBox_Password->GetText().ToString());
	if (!bIsPasswordLongEnough)
	{
		TextBlock_StatusMessagePasswordLongEnough->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		TextBlock_StatusMessagePasswordLongEnough->SetVisibility(ESlateVisibility::Collapsed);
	}
	UpdateSignUpButtonState();
}
void USignUpPage::UpdateStatusMessageConfirmPassword(const FText& Text)
{
	bArePasswordsEqual = TextBox_Password->GetText().ToString() == TextBox_ConfirmPassword->GetText().ToString();
	if (!bArePasswordsEqual)
	{
		TextBlock_StatusMessagePasswordsMatch->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		TextBlock_StatusMessagePasswordsMatch->SetVisibility(ESlateVisibility::Collapsed);
	}
	UpdateSignUpButtonState();
}


bool USignUpPage::IsValidEmail(const FString& Email)
{
	const FRegexPattern EmailPattern(TEXT(R"((^[^\s@]+@[^\s@]+\.[^\s@]{2,}$))"));
	FRegexMatcher Matcher(EmailPattern, Email);
	return Matcher.FindNext();
}

bool USignUpPage::IsStrongPassword(const FString& Password)
{
	const FRegexPattern NumberPattern(TEXT(R"(\d)")); // contains at least one number
	const FRegexPattern SpecialCharPattern(TEXT(R"([^\w\s])")); // contains at least one special character
	const FRegexPattern UppercasePattern(TEXT(R"([A-Z])")); // contains at least one uppercase character
	const FRegexPattern LowercasePattern(TEXT(R"([a-z])")); // contains at least one lowercase character

	FRegexMatcher NumberMatcher(NumberPattern, Password);
	FRegexMatcher SpecialCharMatcher(SpecialCharPattern, Password);
	FRegexMatcher UppercaseMatcher(UppercasePattern, Password);
	FRegexMatcher LowercaseMatcher(LowercasePattern, Password);

	const bool bHasNumber = NumberMatcher.FindNext();
	const bool bHasSpecialChar = SpecialCharMatcher.FindNext();
	const bool bHasUppercase = UppercaseMatcher.FindNext();
	const bool bHasLowercase = LowercaseMatcher.FindNext();

	// Checks for number
	if (!bHasNumber)
	{
		TextBlock_StatusMessagePasswordHasNumber->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		TextBlock_StatusMessagePasswordHasNumber->SetVisibility(ESlateVisibility::Collapsed);
	}
	//Checks for special character
	if (!bHasSpecialChar)
	{
		TextBlock_StatusMessagePasswordSpecialChar->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		TextBlock_StatusMessagePasswordSpecialChar->SetVisibility(ESlateVisibility::Collapsed);
	}
	//Checks for upper case
	if (!bHasUppercase)
	{
		TextBlock_StatusMessagePasswordUppercase->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		TextBlock_StatusMessagePasswordUppercase->SetVisibility(ESlateVisibility::Collapsed);
	}
	//Checks for lower case
	if (!bHasLowercase)
	{
		TextBlock_StatusMessagePasswordLowercase->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		TextBlock_StatusMessagePasswordLowercase->SetVisibility(ESlateVisibility::Collapsed);
	}

	return bHasNumber && bHasSpecialChar && bHasUppercase && bHasLowercase;
}
