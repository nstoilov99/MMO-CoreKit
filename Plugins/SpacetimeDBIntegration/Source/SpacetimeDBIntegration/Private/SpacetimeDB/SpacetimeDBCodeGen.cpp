// Fill out your copyright notice in the Description page of Project Settings.


#include "SpacetimeDB/SpacetimeDBCodeGen.h"
#include "Engine/Engine.h"
#include "UObject/UnrealType.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// Static member definitions
TArray<UStruct*> USpacetimeDBCodeGen::RegisteredTypes;
TArray<UFunction*> USpacetimeDBCodeGen::RegisteredReducers;

void USpacetimeDBCodeGen::RegisterType(UStruct* StructType)
{
    if (StructType && !RegisteredTypes.Contains(StructType))
    {
        RegisteredTypes.Add(StructType);
        UE_LOG(LogTemp, Log, TEXT("Registered SpacetimeDB type: %s"), *StructType->GetName());
    }
}

FString USpacetimeDBCodeGen::GenerateRustCode()
{
    FString RustCode;
    
    // Add file header
    RustCode += TEXT("// Auto-generated SpacetimeDB code from Unreal Engine\n");
    RustCode += TEXT("// DO NOT EDIT MANUALLY\n\n");
    RustCode += TEXT("use spacetimedb::{table, reducer, Identity, Timestamp, ReducerContext};\n");
    RustCode += TEXT("use crate::*;\n\n");
    
    // Generate table structs
    for (UStruct* StructType : RegisteredTypes)
    {
        if (HasSpacetimeDBMetadata(StructType, TEXT("SpacetimeDBTable")))
        {
            RustCode += GenerateTableStruct(StructType);
            RustCode += TEXT("\n\n");
        }
    }
    
    // Generate reducer functions
    for (UFunction* Function : RegisteredReducers)
    {
        RustCode += GenerateReducerFunction(Function);
        RustCode += TEXT("\n\n");
    }
    
    return RustCode;
}

FString USpacetimeDBCodeGen::GenerateReducerBindings(UClass* ClassType)
{
    FString BindingCode;
    
    if (!ClassType)
    {
        return BindingCode;
    }
    
    // Iterate through all functions in the class
    for (TFieldIterator<UFunction> FuncIt(ClassType); FuncIt; ++FuncIt)
    {
        UFunction* Function = *FuncIt;
        
        // Check if function has SpacetimeDB reducer metadata
        if (Function->HasMetaData(TEXT("SpacetimeDBReducer")))
        {
            if (!RegisteredReducers.Contains(Function))
            {
                RegisteredReducers.Add(Function);
            }
        }
    }
    
    return BindingCode;
}

bool USpacetimeDBCodeGen::WriteGeneratedCodeToFile(const FString& OutputPath)
{
    FString GeneratedCode = GenerateRustCode();
    
    if (GeneratedCode.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("No code generated"));
        return false;
    }
    
    // Ensure directory exists
    FString Directory = FPaths::GetPath(OutputPath);
    if (!IFileManager::Get().DirectoryExists(*Directory))
    {
        IFileManager::Get().MakeDirectory(*Directory, true);
    }
    
    // Write to file
    bool bSuccess = FFileHelper::SaveStringToFile(GeneratedCode, *OutputPath);
    
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("Generated SpacetimeDB code written to: %s"), *OutputPath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to write generated code to: %s"), *OutputPath);
    }
    
    return bSuccess;
}

FString USpacetimeDBCodeGen::ConvertUPropertyToRustType(FProperty* Property)
{
    if (!Property)
    {
        return TEXT("()");
    }
    
    // Handle different property types
    if (Property->IsA<FStrProperty>())
    {
        return TEXT("String");
    }
    else if (Property->IsA<FIntProperty>())
    {
        return TEXT("i32");
    }
    else if (Property->IsA<FInt64Property>())
    {
        return TEXT("i64");
    }
    else if (Property->IsA<FFloatProperty>())
    {
        return TEXT("f32");
    }
    else if (Property->IsA<FDoubleProperty>())
    {
        return TEXT("f64");
    }
    else if (Property->IsA<FBoolProperty>())
    {
        return TEXT("bool");
    }
    else if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
    {
        FString StructName = StructProp->Struct->GetName();
        
        // Handle common Unreal types
        if (StructName == TEXT("Vector"))
        {
            return TEXT("(f32, f32, f32)"); // or create a custom Vector3 type
        }
        else if (StructName == TEXT("DateTime"))
        {
            return TEXT("Timestamp");
        }
        else
        {
            // Convert custom struct name
            return FString::Printf(TEXT("%s"), *StructName);
        }
    }
    else if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
    {
        FString InnerType = ConvertUPropertyToRustType(ArrayProp->Inner);
        return FString::Printf(TEXT("Vec<%s>"), *InnerType);
    }
    
    // Default fallback
    return TEXT("String");
}

FString USpacetimeDBCodeGen::GenerateTableStruct(UStruct* StructType)
{
    if (!StructType)
    {
        return TEXT("");
    }
    
    FString TableName = StructType->GetMetaData(TEXT("SpacetimeDBTable"));
    if (TableName.IsEmpty())
    {
        TableName = StructType->GetName().ToLower();
    }
    
    FString RustStruct;
    RustStruct += FString::Printf(TEXT("/// Generated from %s\n"), *StructType->GetName());
    RustStruct += TEXT("#[derive(Clone, Debug)]\n");
    RustStruct += FString::Printf(TEXT("#[table(name = %s, public)]\n"), *TableName);
    RustStruct += FString::Printf(TEXT("pub struct %s {\n"), *StructType->GetName().RightChop(1)); // Remove 'F' prefix
    
    // Iterate through all properties
    for (TFieldIterator<FProperty> PropIt(StructType); PropIt; ++PropIt)
    {
        FProperty* Property = *PropIt;
        
        if (!Property)
        {
            continue;
        }
        
        // Check for SpacetimeDB metadata
        bool bIsPrimaryKey = Property->HasMetaData(TEXT("SpacetimeDBPrimaryKey"));
        bool bIsUnique = Property->HasMetaData(TEXT("SpacetimeDBUnique"));
        
        // Add annotations
        if (bIsPrimaryKey)
        {
            RustStruct += TEXT("    #[primary_key]\n");
        }
        else if (bIsUnique)
        {
            RustStruct += TEXT("    #[unique]\n");
        }
        
        // Convert property to Rust type
        FString RustType = ConvertUPropertyToRustType(Property);
        FString PropertyName = Property->GetName().ToLower();
        
        // Handle special cases for naming conversion
        if (PropertyName.StartsWith(TEXT("b")) && Property->IsA<FBoolProperty>())
        {
            PropertyName = PropertyName.RightChop(1).ToLower(); // Remove 'b' prefix from booleans
        }
        
        RustStruct += FString::Printf(TEXT("    pub %s: %s,\n"), *PropertyName, *RustType);
    }
    
    RustStruct += TEXT("}");
    
    return RustStruct;
}

FString USpacetimeDBCodeGen::GenerateReducerFunction(UFunction* Function)
{
    if (!Function)
    {
        return TEXT("");
    }
    
    FString ReducerName = Function->GetMetaData(TEXT("SpacetimeDBReducer"));
    if (ReducerName.IsEmpty())
    {
        ReducerName = Function->GetName().ToLower();
    }
    
    FString RustFunction;
    RustFunction += FString::Printf(TEXT("/// Generated from %s::%s\n"), 
                                  *Function->GetOuterUClass()->GetName(), 
                                  *Function->GetName());
    RustFunction += TEXT("#[reducer]\n");
    RustFunction += FString::Printf(TEXT("pub fn %s(\n"), *ReducerName);
    RustFunction += TEXT("    ctx: &ReducerContext");
    
    // Add function parameters
    for (TFieldIterator<FProperty> ParamIt(Function); ParamIt; ++ParamIt)
    {
        FProperty* Param = *ParamIt;
        
        if (Param->HasAnyPropertyFlags(CPF_Parm) && 
            !Param->HasAnyPropertyFlags(CPF_ReturnParm))
        {
            FString ParamName = Param->GetName().ToLower();
            FString ParamType = ConvertUPropertyToRustType(Param);
            
            RustFunction += FString::Printf(TEXT(",\n    %s: %s"), *ParamName, *ParamType);
        }
    }
    
    RustFunction += TEXT("\n) -> Result<(), String> {\n");
    RustFunction += TEXT("    // TODO: Implement reducer logic\n");
    RustFunction += TEXT("    // This should be generated from the C++ function body\n");
    RustFunction += TEXT("    // or provided via templates\n");
    RustFunction += TEXT("    Ok(())\n");
    RustFunction += TEXT("}");
    
    return RustFunction;
}

bool USpacetimeDBCodeGen::HasSpacetimeDBMetadata(const UStruct* Struct, const FString& MetaKey)
{
    if (!Struct)
    {
        return false;
    }
    
    return Struct->HasMetaData(*MetaKey);
}