// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayTags/MMONetManagerTags.h"

namespace MMONetManagerTags
{
	namespace AccessPortalAPI
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SignUp, "MMONetManagerTags.AccessPortalAPI.SignUp", "Registers new User in the Access Portal API")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SignIn, "MMONetManagerTags.AccessPortalAPI.SignIn", "Retrieves Access Token & Id Token and Refresh Token Access Portal API")
	}
}