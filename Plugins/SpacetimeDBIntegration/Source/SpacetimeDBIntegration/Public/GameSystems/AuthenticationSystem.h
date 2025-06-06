#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpacetimeDB/SpacetimeDBCodeGen.h"
#include "SpacetimeDB/SpacetimeDBMacros.h"
#include "SpacetimeDB/SpacetimeDBTypes.h"
#include "AuthenticationSystem.generated.h"

// SpacetimeDB User table equivalent in Unreal
USTRUCT(BlueprintType, meta = (SpacetimeDBTable = "user"))
struct SPACETIMEDBINTEGRATION_API FSpacetimeDBUser
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (SpacetimeDBPrimaryKey = "true"))
    FString Identity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (SpacetimeDBUnique = "true"))
    FString Username;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString PasswordHash;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Email;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime CreatedAt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime LastLogin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
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

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPACETIMEDBINTEGRATION_API UAuthenticationSystem : public UActorComponent
{
    GENERATED_BODY()

public:
    UAuthenticationSystem();

    // SpacetimeDB reducer equivalents
    UFUNCTION(BlueprintCallable, meta = (SpacetimeDBReducer = "register_user"))
    bool RegisterUser(const FString& Username, const FString& Password, const FString& Email);

    UFUNCTION(BlueprintCallable, meta = (SpacetimeDBReducer = "login_user"))
    bool LoginUser(const FString& Username, const FString& Password);

    UFUNCTION(BlueprintCallable, meta = (SpacetimeDBReducer = "logout_user"))
    bool LogoutUser();

    UFUNCTION()
    void HandleAuthResult(bool bSuccess, const FString& Message);

    // For code generation demonstration
    UFUNCTION(BlueprintCallable, Category = "Code Generation")
    FString GenerateRustCodeForUser();

    UPROPERTY(BlueprintAssignable, Category = "Authentication")
    FOnUserRegistered OnUserRegistered;

    UPROPERTY(BlueprintAssignable, Category = "Authentication")
    FOnUserLoggedIn OnUserLoggedIn;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    class USpacetimeDBSubsystem* SpacetimeDBSubsystem;

    FString CurrentUsername;
    bool bIsLoggedIn;
};