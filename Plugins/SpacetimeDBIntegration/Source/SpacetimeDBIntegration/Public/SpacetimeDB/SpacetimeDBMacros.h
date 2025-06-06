// SpacetimeDBMacros.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"

// Custom metadata keys for SpacetimeDB integration
#define SPACETIMEDB_TABLE_META TEXT("SpacetimeDBTable")
#define SPACETIMEDB_REDUCER_META TEXT("SpacetimeDBReducer")
#define SPACETIMEDB_PRIMARY_KEY_META TEXT("SpacetimeDBPrimaryKey")
#define SPACETIMEDB_UNIQUE_META TEXT("SpacetimeDBUnique")

// Enhanced USTRUCT macro for SpacetimeDB tables
#define USTRUCT_SPACETIMEDB_TABLE(TableName) \
USTRUCT(BlueprintType, meta = (SpacetimeDBTable = #TableName))

// Enhanced UFUNCTION macro for SpacetimeDB reducers
#define UFUNCTION_SPACETIMEDB_REDUCER(ReducerName) \
UFUNCTION(BlueprintCallable, meta = (SpacetimeDBReducer = #ReducerName))

// Property macros for database fields
#define UPROPERTY_SPACETIMEDB_PRIMARY_KEY() \
UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (SpacetimeDBPrimaryKey = "true"))

#define UPROPERTY_SPACETIMEDB_UNIQUE() \
UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (SpacetimeDBUnique = "true"))

#define UPROPERTY_SPACETIMEDB_FIELD() \
UPROPERTY(EditAnywhere, BlueprintReadWrite)

// Macro for automatic code generation registration
#define REGISTER_SPACETIMEDB_TYPE(TypeName) \
static struct F##TypeName##RegistrationHelper { \
F##TypeName##RegistrationHelper() { \
USpacetimeDBCodeGen::RegisterType(TypeName::StaticStruct()); \
} \
} TypeName##RegistrationInstance;