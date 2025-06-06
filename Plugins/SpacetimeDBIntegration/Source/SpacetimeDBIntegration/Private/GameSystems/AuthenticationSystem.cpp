// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/AuthenticationSystem.h"
#include "SpacetimeDB/SpacetimeDBBridge.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

UAuthenticationSystem::UAuthenticationSystem()
{
    PrimaryComponentTick.bCanEverTick = false;
    bIsLoggedIn = false;
    CurrentUsername = "";
    SpacetimeDBBridge = nullptr;
}

void UAuthenticationSystem::BeginPlay()
{
    Super::BeginPlay();
    
    // Get the SpacetimeDB bridge from the game instance
    if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
    {
        // Try to find existing bridge or create new one
        SpacetimeDBBridge = GameInstance->GetSubsystem<USpacetimeDBBridge>();
        if (!SpacetimeDBBridge)
        {
            SpacetimeDBBridge = NewObject<USpacetimeDBBridge>(GameInstance);
        }
        
        // Bind to auth events
        if (SpacetimeDBBridge)
        {
            SpacetimeDBBridge->OnAuthResult.AddDynamic(this, &UAuthenticationSystem::HandleAuthResult);
        }
    }
}

bool UAuthenticationSystem::RegisterUser(const FString& Username, const FString& Password, const FString& Email)
{
    if (!SpacetimeDBBridge)
    {
        UE_LOG(LogTemp, Error, TEXT("SpacetimeDB Bridge not available"));
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
    
    // Call the bridge function
    return SpacetimeDBBridge->RegisterUser(Username, Password, Email);
}

bool UAuthenticationSystem::LoginUser(const FString& Username, const FString& Password)
{
    if (!SpacetimeDBBridge)
    {
        UE_LOG(LogTemp, Error, TEXT("SpacetimeDB Bridge not available"));
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
    
    // Call the bridge function
    return SpacetimeDBBridge->LoginUser(Username, Password);
}

bool UAuthenticationSystem::LogoutUser()
{
    if (!SpacetimeDBBridge)
    {
        return false;
    }
    
    if (bIsLoggedIn)
    {
        bIsLoggedIn = false;
        CurrentUsername = "";
        
        UE_LOG(LogTemp, Log, TEXT("User logged out"));
        
        // Note: Add actual logout FFI call to your bridge if needed
        // For now, we just update local state
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

// Example of how the macro system would generate code for the struct
// This demonstrates the concept - in practice, this would be auto-generated
FString UAuthenticationSystem::GenerateRustCodeForUser()
{
    FString RustCode = TEXT(R"(
// Auto-generated from FSpacetimeDBUser
#[derive(Clone, Debug)]
#[table(name = user, public)]
pub struct User {
    #[primary_key]
    pub identity: Identity,
    
    #[unique]
    pub username: String,
    
    pub password_hash: String,
    pub email: Option<String>,
    pub created_at: Timestamp,
    pub last_login: Timestamp,
    pub is_active: bool,
}

// Auto-generated reducer from UAuthenticationSystem::RegisterUser
#[reducer]
pub fn register_user(
    ctx: &ReducerContext,
    username: String,
    password: String,
    email: Option<String>
) -> Result<(), String> {
    // Implementation would be generated based on function body analysis
    // or user-provided templates
    
    // Input validation
    if username.len() < 3 {
        return Err("Username must be at least 3 characters".to_string());
    }
    
    if password.len() < 8 {
        return Err("Password must be at least 8 characters".to_string());
    }
    
    // Check if username exists
    if User::filter_by_username(ctx, &username).is_some() {
        return Err("Username already taken".to_string());
    }
    
    // Create user
    let salt = generate_salt(&ctx.sender, ctx.timestamp);
    let password_hash = hash_password(&password, &salt);
    
    ctx.db.user().insert(User {
        identity: ctx.sender,
        username,
        password_hash,
        email,
        created_at: ctx.timestamp,
        last_login: ctx.timestamp,
        is_active: true,
    });
    
    Ok(())
}
)");
    
    return RustCode;
}