// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AccessPortal/Shared/StatusLogWidget.h"

#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "UI/AccessPortal/Shared/StatusLogMessageRow.h"

void UStatusLogWidget::AddStatusMessage(const FString& Message)
{
	if (!MessageRowClass || !StatusLogBox) return;

	if (UStatusLogMessageRow* NewRow = CreateWidget<UStatusLogMessageRow>(this, MessageRowClass))
	{
		NewRow->SetStatusLogMessage(Message);

		StatusLogBox->AddChildToVerticalBox(NewRow);
		ActiveMessages.Add(NewRow);

		if (ActiveMessages.Num() > MaxMessages)
		{
			UUserWidget* Oldest = ActiveMessages[0];
			StatusLogBox->RemoveChild(Oldest);
			ActiveMessages.RemoveAt(0);
		}
	}
}
