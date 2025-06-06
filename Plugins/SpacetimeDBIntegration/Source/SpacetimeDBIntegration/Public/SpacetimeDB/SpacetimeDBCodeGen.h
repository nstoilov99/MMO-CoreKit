#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/Engine.h"
#include "SpacetimeDBCodeGen.generated.h"

UCLASS()
class SPACETIMEDBINTEGRATION_API USpacetimeDBCodeGen : public UObject
{
	GENERATED_BODY()
    
public:
	// Register a struct as a SpacetimeDB table (manual registration)
	UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|CodeGen")
	static void RegisterStruct(UScriptStruct* StructType);

	// Register a class as a SpacetimeDB handler
	UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|CodeGen")
	static void RegisterClass(UClass* ClassType);

	// Generate Rust code for all registered types
	UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|CodeGen")
	static FString GenerateRustCode();

	// Generate reducer bindings
	UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|CodeGen")
	static FString GenerateReducerBindings(UClass* ClassType);

	// Write generated code to files
	UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|CodeGen")
	static bool WriteGeneratedCodeToFile(const FString& OutputPath);

	// Initialize code generation (call this at startup)
	UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|CodeGen")
	static void InitializeCodeGeneration();

private:
	static TArray<UScriptStruct*> RegisteredStructs;
	static TArray<UClass*> RegisteredClasses;
	static TArray<UFunction*> RegisteredReducers;

	// Helper functions
	static FString ConvertUPropertyToRustType(FProperty* Property);
	static FString GenerateTableStruct(UScriptStruct* StructType);
	static FString GenerateReducerFunction(UFunction* Function);
	static bool HasSpacetimeDBMetadata(const UStruct* Struct, const FString& MetaKey);
	static void DiscoverTypesAutomatically();
};