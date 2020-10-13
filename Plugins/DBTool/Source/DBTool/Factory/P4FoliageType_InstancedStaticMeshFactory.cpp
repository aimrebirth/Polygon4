// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "P4FoliageType_InstancedStaticMeshFactory.h"
#include "FoliageType_InstancedStaticMesh.h"
#include "FoliageType_Actor.h"

#define LOCTEXT_NAMESPACE "FoliageTypeFactory"

UP4FoliageType_InstancedStaticMeshFactory::UP4FoliageType_InstancedStaticMeshFactory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = UFoliageType_InstancedStaticMesh::StaticClass();
}

UObject* UP4FoliageType_InstancedStaticMeshFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    UFoliageType_InstancedStaticMesh *NewFoliageType;
    NewFoliageType = NewObject<UFoliageType_InstancedStaticMesh>(InParent, Class, Name, Flags | RF_Transactional);
    NewFoliageType->AlignToNormal = 0;
    NewFoliageType->AlignMaxAngle = 0;
    NewFoliageType->RandomYaw = 0;
    NewFoliageType->RandomPitchAngle = 0;
    NewFoliageType->GroundSlopeAngle.Max = 90;
    NewFoliageType->Mobility = EComponentMobility::Static;
    NewFoliageType->CullDistance.Max = 15000;
    NewFoliageType->BodyInstance.bEnableGravity = false;
    NewFoliageType->BodyInstance.bSimulatePhysics = false;
    NewFoliageType->BodyInstance.bUseCCD = false;
    FString String = "BlockAll";
    NewFoliageType->BodyInstance.SetCollisionProfileNameDeferred(FName("BlockAll"));
    return NewFoliageType;
}

FText UP4FoliageType_InstancedStaticMeshFactory::GetToolTip() const
{
    return LOCTEXT("FoliageTypeStaticMeshToolTip", "<CUSTOM FACTORY!> Static Mesh Foliage is a foliage type that will use mesh instancing and is optimal for non-interactive foliage.");
}

