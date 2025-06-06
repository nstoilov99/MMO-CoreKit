#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpacetimeDB/SpacetimeDBCodeGen.h"
#include "SpacetimeDB/SpacetimeDBMacros.h"
#include "AuthenticationSystem.generated.h"

// SpacetimeDB User table equivalent in Unreal
USTRUCT_SPACETIMEDB_TABLE(user)
struct SPACETIMEDBINTEGRATION_API FSpacetimeDBUser
{

    UPROPERTY_SPACETIMEDB_PRIMARY_KEY()
    FString Identity;

    UPROPERTY_SPACETIMEDB_UNIQUE()
    FString Username;

    UPROPERTY_SPACETIMEDB_FIELD()
    FString PasswordHash;

    UPROPERTY_SPACETIMEDB_FIELD()
    FString Email;

    UPROPERTY_SPACETIMEDB_FIELD()
    FDateTime CreatedAt;

    UPROPERTY_SPACETIMEDB_FIELD()
    FDateTime LastLogin;

    UPROPERTY_SPACETIMEDB_FIELD()
    bool bIsActive;

    FSpacetimeDBUser()
    {
        Identity = "";
        Username = "";
        PasswordHash = "";
        Email = "";
        CreatedAt = FDateTime::Now();
        LastLogin = FDateTime::Now();
        bIsActive = true;
    }
};

// Events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserRegistered, bool, bSuccess, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserLoggedIn, bool, bSuccess, const FString&, Message);

// Register the type for code generation


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPACETIMEDBINTEGRATION_API UAuthenticationSystem : public UActorComponent
{
    GENERATED_BODY()

public:
    UAuthenticationSystem();

    // SpacetimeDB reducer equivalents
    UFUNCTION_SPACETIMEDB_REDUCER(register_user)
    bool RegisterUser(const FString& Username, const FString& Password, const FString& Email);

    UFUNCTION_SPACETIMEDB_REDUCER(login_user)
    bool LoginUser(const FString& Username, const FString& Password);

    UFUNCTION_SPACETIMEDB_REDUCER(logout_user)
    bool LogoutUser();

    UFUNCTION_SPACETIMEDB_REDUCER(authenticate_user)
    void HandleAuthResult(bool bSuccess, const FString& Message);

    
    FString GenerateRustCodeForUser();

    UPROPERTY(BlueprintAssignable, Category = "Authentication")
    FOnUserRegistered OnUserRegistered;

    UPROPERTY(BlueprintAssignable, Category = "Authentication")
    FOnUserLoggedIn OnUserLoggedIn;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    class USpacetimeDBBridge* SpacetimeDBBridge;

    FString CurrentUsername;
    bool bIsLoggedIn;
};