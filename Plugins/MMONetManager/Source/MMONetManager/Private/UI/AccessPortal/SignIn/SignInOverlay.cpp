// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AccessPortal/SignIn/SignInOverlay.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "UI/AccessPortal/AccessPortalManager.h"
#include "UI/AccessPortal/SignIn/SignUpPage.h"

void USignInOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	AccessPortalManager = NewObject<UAccessPortalManager>(this, AccessPortalManagerClass);

	SignUpPage->Button_SignUp->OnClicked.AddDynamic(this, &USignInOverlay::SignUpButtonClicked);
}

void USignInOverlay::SignUpButtonClicked()
{
	const FString Username = SignUpPage->TextBox_UserName->GetText().ToString();
	const FString Password = SignUpPage->TextBox_Password->GetText().ToString();
	const FString Email = SignUpPage->TextBox_Email->GetText().ToString();
	AccessPortalManager->SignUp(Username, Password, Email);
}
