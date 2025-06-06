#include "GameSystems/AuthenticationSystem.h"
#include "SpacetimeDB/SpacetimeDBSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

UAuthenticationSystem::UAuthenticationSystem()
{
    PrimaryComponentTick.bCanEverTick = false;
    bIsLoggedIn = false;
    CurrentUsername = "";
    SpacetimeDBSubsystem = nullptr;
}

void UAuthenticationSystem::BeginPlay()
{
    Super::BeginPlay();
    
    // Get the SpacetimeDB subsystem from the game instance
    if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
    {
        SpacetimeDBSubsystem = GameInstance->GetSubsystem<USpacetimeDBSubsystem>();
        
        // Bind to auth events
        if (SpacetimeDBSubsystem)
        {
            SpacetimeDBSubsystem->OnUserAuthenticated.AddDynamic(this, &UAuthenticationSystem::HandleAuthResult);
            SpacetimeDBSubsystem->OnUserRegistered.AddDynamic(this, &UAuthenticationSystem::HandleAuthResult);
        }
    }
}

bool UAuthenticationSystem::RegisterUser(const FString& Username, const FString& Password, const FString& Email)
{
    if (!SpacetimeDBSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("SpacetimeDB Subsystem not available"));
        OnUserRegistered.Broadcast(false, TEXT("Service unavailable"));
        return false;
    }
    
    // Validate input
    if (Username.Len() < 3)
    {
        OnUserRegistered.Broadcast(false, TEXT("Username must be at least 3 characters"));
        return false;
    }
    
    if (Password.Len() < 8)
    {
        OnUserRegistered.Broadcast(false, TEXT("Password must be at least 8 characters"));
        return false;
    }
    
    // Check for valid characters in username
    for (int32 i = 0; i < Username.Len(); i++)
    {
        TCHAR c = Username[i];
        if (!FChar::IsAlnum(c) && c != '_')
        {
            OnUserRegistered.Broadcast(false, TEXT("Username can only contain letters, numbers, and underscores"));
            return false;
        }
    }
    
    // Validate email if provided
    if (!Email.IsEmpty())
    {
        if (!Email.Contains(TEXT("@")) || !Email.Contains(TEXT(".")))
        {
            OnUserRegistered.Broadcast(false, TEXT("Invalid email format"));
            return false;
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("Attempting to register user: %s"), *Username);
    
    // Store the username for the callback
    CurrentUsername = Username;
    
    // Call the subsystem function
    SpacetimeDBSubsystem->RegisterUser(Username, Password, Email);
    return true;
}

bool UAuthenticationSystem::LoginUser(const FString& Username, const FString& Password)
{
    if (!SpacetimeDBSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("SpacetimeDB Subsystem not available"));
        OnUserLoggedIn.Broadcast(false, TEXT("Service unavailable"));
        return false;
    }
    
    if (Username.IsEmpty() || Password.IsEmpty())
    {
        OnUserLoggedIn.Broadcast(false, TEXT("Username and password are required"));
        return false;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Attempting to login user: %s"), *Username);
    
    // Store the username for the callback
    CurrentUsername = Username;
    
    // Call the subsystem function
    SpacetimeDBSubsystem->LoginUser(Username, Password);
    return true;
}

bool UAuthenticationSystem::LogoutUser()
{
    if (!SpacetimeDBSubsystem)
    {
        return false;
    }
    
    if (bIsLoggedIn)
    {
        bIsLoggedIn = false;
        CurrentUsername = "";
        
        UE_LOG(LogTemp, Log, TEXT("User logged out"));
        
        SpacetimeDBSubsystem->LogoutUser();
        return true;
    }
    
    return false;
}

void UAuthenticationSystem::HandleAuthResult(bool bSuccess, const FString& Message)
{
    if (bSuccess)
    {
        bIsLoggedIn = true;
        UE_LOG(LogTemp, Log, TEXT("Authentication successful for user: %s"), *CurrentUsername);
        
        // Broadcast success
        OnUserRegistered.Broadcast(true, TEXT("Success"));
        OnUserLoggedIn.Broadcast(true, TEXT("Login successful"));
    }
    else
    {
        bIsLoggedIn = false;
        UE_LOG(LogTemp, Warning, TEXT("Authentication failed: %s"), *Message);
        
        // Broadcast failure
        OnUserRegistered.Broadcast(false, Message);
        OnUserLoggedIn.Broadcast(false, Message);
    }
}

FString UAuthenticationSystem::GenerateRustCodeForUser()
{
    // This would typically be called by the code generator
    return USpacetimeDBCodeGen::GenerateRustCode();
}