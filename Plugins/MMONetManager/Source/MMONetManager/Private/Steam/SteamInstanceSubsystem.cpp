// Fill out your copyright notice in the Description page of Project Settings.


#include "Steam/SteamInstanceSubsystem.h"

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

USteamInstanceSubsystem::USteamInstanceSubsystem()
{
	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		IdentityInterface = Subsystem->GetIdentityInterface();
		if (IdentityInterface.IsValid())
		{
			SteamId = IdentityInterface->GetUniquePlayerId(0);
			Username = IdentityInterface->GetPlayerNickname(*SteamId);
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Magenta, FString::Printf(TEXT("Steam Access Token: %s"), *IdentityInterface->GetAuthToken(0)));
			}
		}
	}
}

FString USteamInstanceSubsystem::GetSteamUsername() const
{
	if (!IdentityInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Identity Interface is not valid!"));
		return FString("");
	}

	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
	{
		TSharedPtr<const FUniqueNetId> UserId = LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId();

		return IdentityInterface->GetPlayerNickname(*UserId);
	}

	return FString("");
}
