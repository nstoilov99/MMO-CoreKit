#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "UObject/NoExportTypes.h"
#include "SpacetimeDBTypes.generated.h"

// Forward declarations for FFI functions
extern "C" {
    struct FFIResult {
        bool success;
        char* error_message;
        void* data;
        size_t data_size;
    };
    
    // Connection management
    FFIResult spacetimedb_connect(const char* host, uint16_t port, const char* database_name);
    
    // Authentication
    FFIResult spacetimedb_register_user(const char* username, const char* password, const char* email);
    FFIResult spacetimedb_login_user(const char* username, const char* password);
    
    // Game operations
    FFIResult spacetimedb_join_game(const char* starting_zone);
    FFIResult spacetimedb_update_position(float x, float y, float z, float yaw);
    FFIResult spacetimedb_send_chat(const char* message, const char* channel);
    
    // Memory management
    void spacetimedb_free_result(FFIResult* result);
    void spacetimedb_free_string(char* ptr);
}

/**
 * Base class for SpacetimeDB table representations
 */
UCLASS(BlueprintType, Abstract)
class SPACETIMEDBINTEGRATION_API USpacetimeDBTable : public UObject
{
    GENERATED_BODY()

public:
    USpacetimeDBTable();
    
    virtual FString SerializeToJson() const;
    virtual bool DeserializeFromJson(const FString& JsonString);
    
    static FString GetTableName(const UClass* TableClass);
    static TArray<FString> GetPrimaryKeyFields(const UClass* TableClass);
    static TArray<FString> GetUniqueFields(const UClass* TableClass);
};

/**
 * Player data structure
 */
USTRUCT(BlueprintType, meta = (SpacetimeDBTable = "game_players"))
struct SPACETIMEDBINTEGRATION_API FSpacetimeDBPlayer
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (SpacetimeDBPrimaryKey = "true"))
    int64 Id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Username;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Position;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Health;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsOnline;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CurrentZone;

    FSpacetimeDBPlayer()
        : Id(0)
        , Username("")
        , Position(FVector::ZeroVector)
        , Rotation(0.0f)
        , Level(1)
        , Health(100.0f)
        , MaxHealth(100.0f)
        , bIsOnline(false)
        , CurrentZone("default")
    {}
};

/**
 * Chat message structure
 */
USTRUCT(BlueprintType, meta = (SpacetimeDBTable = "chatmessage"))
struct SPACETIMEDBINTEGRATION_API FSpacetimeDBChatMessage
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (SpacetimeDBPrimaryKey = "true"))
    int64 MessageId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString SenderUsername;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Message;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Channel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime Timestamp;

    FSpacetimeDBChatMessage()
        : MessageId(0)
        , SenderUsername("")
        , Message("")
        , Channel("global")
        , Timestamp(FDateTime::Now())
    {}
};

/**
 * Result wrapper for async operations
 */
USTRUCT(BlueprintType)
struct SPACETIMEDBINTEGRATION_API FSpacetimeDBResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    bool bSuccess;

    UPROPERTY(BlueprintReadOnly)
    FString ErrorMessage;

    UPROPERTY(BlueprintReadOnly)
    FString Data;

    FSpacetimeDBResult()
        : bSuccess(false)
        , ErrorMessage("")
        , Data("")
    {}

    static FSpacetimeDBResult Success(const FString& InData = "")
    {
        FSpacetimeDBResult Result;
        Result.bSuccess = true;
        Result.Data = InData;
        return Result;
    }

    static FSpacetimeDBResult Error(const FString& InErrorMessage)
    {
        FSpacetimeDBResult Result;
        Result.bSuccess = false;
        Result.ErrorMessage = InErrorMessage;
        return Result;
    }
};

/**
 * Connection states
 */
UENUM(BlueprintType)
enum class ESpacetimeDBConnectionState : uint8
{
    Disconnected,
    Connecting,
    Connected,
    Authenticating,
    Authenticated,
    InGame,
    Error
};