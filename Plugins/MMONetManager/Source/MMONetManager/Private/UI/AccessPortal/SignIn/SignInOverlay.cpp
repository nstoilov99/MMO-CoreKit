// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AccessPortal/SignIn/SignInOverlay.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/WidgetSwitcher.h"
#include "UI/AccessPortal/AccessPortalManager.h"
#include "UI/AccessPortal/Shared/StatusLogWidget.h"
#include "UI/AccessPortal/SignIn/SignInPage.h"
#include "UI/AccessPortal/SignIn/SignUpPage.h"

void USignInOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	AccessPortalManager = NewObject<UAccessPortalManager>(this, AccessPortalManagerClass);
	AccessPortalManager->SignUpStatusMessageDelegate.AddDynamic(this, &USignInOverlay::SignUpStatusMessage);
	AccessPortalManager->OnSignUpSucceeded.AddDynamic(this, &USignInOverlay::ShowSignInPage);

	SignInPage->Button_SignUp->OnClicked.AddDynamic(this, &USignInOverlay::ShowSignUpPage);
	SignInPage->Button_SignIn->OnClicked.AddDynamic(this, &USignInOverlay::SignInButtonClicked);

	SignUpPage->Button_SignUp->OnClicked.AddDynamic(this, &USignInOverlay::SignUpButtonClicked);
	SignUpPage->Button_Back->OnClicked.AddDynamic(this, &USignInOverlay::ShowSignInPage);
	ShowSignInPage();
}

void USignInOverlay::SignUpButtonClicked()
{
	const FString Username = SignUpPage->TextBox_UserName->GetText().ToString();
	const FString Password = SignUpPage->TextBox_Password->GetText().ToString();
	const FString Email = SignUpPage->TextBox_Email->GetText().ToString();
	AccessPortalManager->PlayFabSignUp(Username, Password, Email);
}

void USignInOverlay::SignUpStatusMessage(const FString& StatusMessage)
{
	StatusLog->AddStatusMessage(StatusMessage);
}

void USignInOverlay::SignInButtonClicked()
{
	const FString Username = SignInPage->TextBox_UserName->GetText().ToString();
	const FString Password = SignInPage->TextBox_Password->GetText().ToString();

	AccessPortalManager->SignIn(Username, Password);
}

void USignInOverlay::ShowSignInPage()
{
	WidgetSwitcher->SetActiveWidget(SignInPage);
}

void USignInOverlay::ShowSignUpPage()
{
	WidgetSwitcher->SetActiveWidget(SignUpPage);
}
