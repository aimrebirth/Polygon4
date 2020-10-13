// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

/**
* Factory for FoliageType assets
*/

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Factories/Factory.h"
#include "P4FoliageType_InstancedStaticMeshFactory.generated.h"

UCLASS()
class DBTOOL_API UP4FoliageType_InstancedStaticMeshFactory : public UFactory
{
    GENERATED_UCLASS_BODY()

    // UFactory interface
    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
    virtual FString GetDefaultNewAssetName() const override
    {
        return TEXT("NewInstancedStaticMeshFoliage");
    }
    virtual FText GetToolTip() const override;
    // End of UFactory interface
};

