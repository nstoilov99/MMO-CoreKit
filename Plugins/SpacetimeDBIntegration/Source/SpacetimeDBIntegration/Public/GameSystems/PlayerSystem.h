#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpacetimeDB/SpacetimeDBCodeGen.h"
#include "SpacetimeDB/SpacetimeDBMacros.h"
#include "PlayerSystem.generated.h"

// SpacetimeDB Player table equivalent
USTRUCT_SPACETIMEDB_TABLE(game_players)
struct SPACETIMEDBINTEGRATION_API FSpacetimeDBPlayer
{

    UPROPERTY_SPACETIMEDB_PRIMARY_KEY()
    FString Identity;

    UPROPERTY_SPACETIMEDB_FIELD()
    FString Username;

    UPROPERTY_SPACETIMEDB_FIELD()
    FVector Position;

    UPROPERTY_SPACETIMEDB_FIELD()
    float RotationYaw;

    UPROPERTY_SPACETIMEDB_FIELD()
    int32 Level;

    UPROPERTY_SPACETIMEDB_FIELD()
    int64 Experience;

    UPROPERTY_SPACETIMEDB_FIELD()
    float Health;

    UPROPERTY_SPACETIMEDB_FIELD()
    float MaxHealth;

    UPROPERTY_SPACETIMEDB_FIELD()
    bool bIsOnline;

    UPROPERTY_SPACETIMEDB_FIELD()
    FString CurrentZone;

    FSpacetimeDBPlayer()
    {
        Identity = "";
        Username = "";
        Position = FVector::ZeroVector;
        RotationYaw = 0.0f;
        Level = 1;
        Experience = 0;
        Health = 100.0f;
        MaxHealth = 100.0f;
        bIsOnline = false;
        CurrentZone = "";
    }
};

REGISTER_SPACETIMEDB_TYPE(FSpacetimeDBPlayer)

UCLASS()
class SPACETIMEDBINTEGRATION_API APlayerSystem : public AActor
{
    GENERATED_BODY()

public:
    APlayerSystem();

    // SpacetimeDB reducer equivalents
    UFUNCTION_SPACETIMEDB_REDUCER(join_game)
    bool JoinGame(const FString& StartingZone);

    UFUNCTION_SPACETIMEDB_REDUCER(update_player_position)
    bool UpdatePosition(const FVector& NewPosition, float NewYaw);

    UFUNCTION_SPACETIMEDB_REDUCER(leave_game)
    bool LeaveGame();

    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoinedGame, const FSpacetimeDBPlayer&, Player);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerPositionUpdated, const FString&, PlayerIdentity, const FVector&, Position);

    UPROPERTY(BlueprintAssignable, Category = "Player System")
    FOnPlayerJoinedGame OnPlayerJoinedGame;

    UPROPERTY(BlueprintAssignable, Category = "Player System")
    FOnPlayerPositionUpdated OnPlayerPositionUpdated;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY()
    class USpacetimeDBBridge* SpacetimeDBBridge;

    FSpacetimeDBPlayer CurrentPlayer;
    FVector LastSentPosition;
    float LastSentYaw;
    float PositionUpdateThreshold;
    float LastPositionUpdateTime;
};