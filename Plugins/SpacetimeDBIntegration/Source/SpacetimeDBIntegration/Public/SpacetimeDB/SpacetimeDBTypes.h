#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "UObject/NoExportTypes.h"
#include "SpacetimeDBTypes.generated.h"

// Forward declarations for our FFI functions
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

// Custom metadata tags for SpacetimeDB integration
// These are used by our code generation system
#define SPACETIMEDB_TABLE TEXT("SpacetimeDBTable")
#define SPACETIMEDB_REDUCER TEXT("SpacetimeDBReducer")
#define SPACETIMEDB_PRIMARY_KEY TEXT("SpacetimeDBPrimaryKey")
#define SPACETIMEDB_UNIQUE TEXT("SpacetimeDBUnique")

/**
 * Base class for SpacetimeDB table representations
 * This provides common functionality for all SpacetimeDB-backed data
 */
UCLASS(BlueprintType, Abstract)
class SPACETIMEDBINTEGRATION_API USpacetimeDBTable : public UObject
{
    GENERATED_BODY()

public:
    USpacetimeDBTable();
    
    // Serialization support for sending data over the network
    virtual FString SerializeToJson() const;
    virtual bool DeserializeFromJson(const FString& JsonString);
    
    // Metadata access for code generation
    static FString GetTableName(const UClass* TableClass);
    static TArray<FString> GetPrimaryKeyFields(const UClass* TableClass);
    static TArray<FString> GetUniqueFields(const UClass* TableClass);
};

/**
 * Player data structure that mirrors our Rust Player table
 * This demonstrates how to use our custom metadata
 */
USTRUCT(BlueprintType, meta = (SpacetimeDBTable = "true"))
struct SPACETIMEDBINTEGRATION_API FSpacetimeDBPlayer
{
    GENERATED_BODY()

    /** Unique identifier for this player */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (SpacetimeDBPrimaryKey = "true"))
    int64 Id;

    /** Player's display name */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Username;

    /** Player's position in the world */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Position;

    /** Player's rotation (yaw only for simplicity) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Rotation;

    /** Player's current level */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level;

    /** Player's current health */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Health;

    /** Player's maximum health */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxHealth;

    /** Whether this player is currently online */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsOnline;

    /** Current zone or area */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CurrentZone;

    // Constructor with default values
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
USTRUCT(BlueprintType, meta = (SpacetimeDBTable = "true"))
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
 * This provides a consistent way to handle success/failure in Blueprints
 */
USTRUCT(BlueprintType)
struct SPACETIMEDBBRIDGE_API FSpacetimeDBResult
{
    GENERATED_BODY()

    /** Whether the operation succeeded */
    UPROPERTY(BlueprintReadOnly)
    bool bSuccess;

    /** Error message if the operation failed */
    UPROPERTY(BlueprintReadOnly)
    FString ErrorMessage;

    /** Optional data returned by the operation */
    UPROPERTY(BlueprintReadOnly)
    FString Data;

    FSpacetimeDBResult()
        : bSuccess(false)
        , ErrorMessage("")
        , Data("")
    {}

    // Convenience constructors
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
 * Enum for different connection states
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