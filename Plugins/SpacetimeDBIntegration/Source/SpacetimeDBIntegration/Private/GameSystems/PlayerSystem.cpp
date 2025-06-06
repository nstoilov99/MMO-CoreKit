#include "GameSystems/PlayerSystem.h"
#include "SpacetimeDB/SpacetimeDBSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

APlayerSystem::APlayerSystem()
{
    PrimaryActorTick.bCanEverTick = true;
    LastSentPosition = FVector::ZeroVector;
    LastSentYaw = 0.0f;
    LastPositionUpdateTime = 0.0f;
}

void APlayerSystem::BeginPlay()
{
    Super::BeginPlay();
    
    // Get the SpacetimeDB subsystem
    if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
    {
        SpacetimeDBSubsystem = GameInstance->GetSubsystem<USpacetimeDBSubsystem>();
        
        if (SpacetimeDBSubsystem)
        {
            // Bind to subsystem events
            SpacetimeDBSubsystem->OnPlayerJoinedGame.AddDynamic(this, &APlayerSystem::HandlePlayerJoined);
        }
    }
}

void APlayerSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Update position periodically
    LastPositionUpdateTime += DeltaTime;
    if (LastPositionUpdateTime >= 0.1f) // Update every 100ms
    {
        FVector CurrentPosition = GetActorLocation();
        float CurrentYaw = GetActorRotation().Yaw;
        
        if (FVector::Dist(CurrentPosition, LastSentPosition) > PositionUpdateThreshold ||
            FMath::Abs(CurrentYaw - LastSentYaw) > 5.0f)
        {
            UpdatePosition(CurrentPosition, CurrentYaw);
        }
        
        LastPositionUpdateTime = 0.0f;
    }
}

bool APlayerSystem::JoinGame(const FString& StartingZone)
{
    if (!SpacetimeDBSubsystem || !SpacetimeDBSubsystem->IsAuthenticated())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot join game: Not authenticated"));
        return false;
    }
    
    SpacetimeDBSubsystem->JoinGame(StartingZone);
    return true;
}

bool APlayerSystem::UpdatePosition(const FVector& NewPosition, float NewYaw)
{
    if (!SpacetimeDBSubsystem || !SpacetimeDBSubsystem->IsInGame())
    {
        return false;
    }
    
    SpacetimeDBSubsystem->UpdatePlayerPosition(NewPosition, NewYaw);
    LastSentPosition = NewPosition;
    LastSentYaw = NewYaw;
    
    return true;
}

bool APlayerSystem::LeaveGame()
{
    if (!SpacetimeDBSubsystem)
    {
        return false;
    }
    
    SpacetimeDBSubsystem->LeaveGame();
    return true;
}

void APlayerSystem::HandlePlayerJoined(const FSpacetimeDBPlayer& Player)
{
    OnPlayerJoinedGame.Broadcast(Player);
}