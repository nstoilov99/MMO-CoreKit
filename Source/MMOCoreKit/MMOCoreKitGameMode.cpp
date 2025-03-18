// Copyright Epic Games, Inc. All Rights Reserved.

#include "MMOCoreKitGameMode.h"
#include "MMOCoreKitCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMMOCoreKitGameMode::AMMOCoreKitGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
