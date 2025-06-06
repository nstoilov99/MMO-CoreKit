// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SpacetimeDBCodeGen.generated.h"

/**
 * 
 */
UCLASS()
class SPACETIMEDBINTEGRATION_API USpacetimeDBCodeGen : public UObject
{
	GENERATED_BODY()
	
public:
	// Register a struct as a SpacetimeDB table
	static void RegisterType(UStruct* StructType);

	// Generate Rust code for all registered types
	UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|CodeGen")
	static FString GenerateRustCode();

	// Generate reducer bindings
	UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|CodeGen")
	static FString GenerateReducerBindings(UClass* ClassType);

	// Write generated code to files
	UFUNCTION(BlueprintCallable, Category = "SpacetimeDB|CodeGen")
	static bool WriteGeneratedCodeToFile(const FString& OutputPath);

private:
	static TArray<UStruct*> RegisteredTypes;
	static TArray<UFunction*> RegisteredReducers;

	// Helper functions
	static FString ConvertUPropertyToRustType(FProperty* Property);
	static FString GenerateTableStruct(UStruct* StructType);
	static FString GenerateReducerFunction(UFunction* Function);
	static bool HasSpacetimeDBMetadata(const UStruct* Struct, const FString& MetaKey);
	
};
