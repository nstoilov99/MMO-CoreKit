#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpacetimeDB/SpacetimeDBTypes.h"
#include "PlayerSystem.generated.h"

// Events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoinedGame, const FSpacetimeDBPlayer&, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerPositionUpdated, const FString&, PlayerIdentity, const FVector&, Position);

UCLASS()
class SPACETIMEDBINTEGRATION_API APlayerSystem : public AActor
{
    GENERATED_BODY()

public:
    APlayerSystem();

    // SpacetimeDB reducer equivalents
    UFUNCTION(BlueprintCallable, meta = (SpacetimeDBReducer = "join_game"))
    bool JoinGame(const FString& StartingZone);

    UFUNCTION(BlueprintCallable, meta = (SpacetimeDBReducer = "update_player_position"))
    bool UpdatePosition(const FVector& NewPosition, float NewYaw);

    UFUNCTION(BlueprintCallable, meta = (SpacetimeDBReducer = "leave_game"))
    bool LeaveGame();
    
    void HandlePlayerJoined(const FSpacetimeDBPlayer& Player);

    UPROPERTY(BlueprintAssignable, Category = "Player System")
    FOnPlayerJoinedGame OnPlayerJoinedGame;

    UPROPERTY(BlueprintAssignable, Category = "Player System")
    FOnPlayerPositionUpdated OnPlayerPositionUpdated;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY()
    class USpacetimeDBSubsystem* SpacetimeDBSubsystem;

    FSpacetimeDBPlayer CurrentPlayer;
    FVector LastSentPosition;
    float LastSentYaw;
    
    UPROPERTY(EditAnywhere, Category = "Player System")
    float PositionUpdateThreshold = 1.0f;
    
    float LastPositionUpdateTime;
};