/*
 * Polygon-4
 * Copyright (C) 2019 Yakim3396
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "P4GliderMovementComponent.h"

#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "P4FunctionLibrary.h"
#include "PhysicsEngine/PhysicsSettings.h"

UP4GliderMovementComponent::UP4GliderMovementComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    bAutoActivate = true;
    bWantsInitializeComponent = true;
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
    SetIsReplicatedByDefault(true);

    UpdatedMesh = nullptr;

    // PRIVATE
    bItsChecksOk = false;
    RefFrameTransformCenter;
    bContactPointActive = false;
    STLocation = FVector::ZeroVector;
    STImpactPoint = FVector::ZeroVector;
    STImpactNormal = FVector::ZeroVector;
    STPhysMaterial = nullptr;

    HoverTraceDirection = FVector::UpVector;
    Offset = 0.f;
    CurrentHeight = 0.f;
    PreviousHeight = MaxHeightHover;

    SphereCheckShape = FCollisionShape::MakeSphere(STRadius);
    // PRIVATE
}

//////////////////////////////////////////////////////////////////////////
// Initialization

void UP4GliderMovementComponent::InitializeComponent()
{
    Super::InitializeComponent();
    InitMesh();
    InitBodyPhysics();
}

//////////////////////////////////////////////////////////////////////////
// Physics Initialization

void UP4GliderMovementComponent::InitMesh()
{
    if (!UpdatedMesh)
    {
        bItsChecksOk = false;
        UpdatedMesh = Cast<UStaticMeshComponent>(UpdatedComponent);
    }
}

void UP4GliderMovementComponent::InitBodyPhysics()
{
    if (!UpdatedMesh)
    {
        bItsChecksOk = false;
        UE_LOG(Polygon4, Error, TEXT("InitBodyPhysics failed: No UpdatedMesh component found"));
        return;
    }
    bItsChecksOk = true;
    UpdatedMesh->SetMassOverrideInKg(NAME_None, 1000.f);
    UpdatedMesh->SetLinearDamping(0.f);
    UpdatedMesh->SetAngularDamping(0.f);
    UpdatedMesh->ScaleByMomentOfInertia(FVector(0.f, 0.f, 0.f));

    FTransform TransformCenter = UpdatedMesh->GetComponentTransform();
    FTransform TransformSocket = UpdatedMesh->GetSocketTransform(HoverTraceStart);
    Offset = (TransformCenter.GetLocation() - TransformSocket.GetLocation()).Size();
}

void UP4GliderMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bItsChecksOk)
        return;
    STCheckGround();
    if (!bIsEnabled)
        return;
    UpdatePhysics(DeltaTime);
}

void UP4GliderMovementComponent::UpdatePhysics(const float DT)
{
    FVector AirDragFinForce = FVector::ZeroVector;
    FVector HoverFinForce = FVector::ZeroVector;
    FVector FrictionFinForce = FVector::ZeroVector;
    if (bContactPointActive)
    {
        CalcForceHover(DT, HoverFinForce);
        if (bIsFriction)
        {
            CalcForceFriction(DT, FrictionFinForce);
        }
    }
    CalcForceAirDrag(DT, AirDragFinForce);
    UpdatedMesh->AddForce((HoverFinForce + AirDragFinForce + FrictionFinForce) * 100.f);
}

void UP4GliderMovementComponent::STCheckGround()
{
    RefFrameTransformCenter = UpdatedMesh->GetComponentTransform();

    FHitResult SphereTraceOutHit;
    FVector SphereTraceStart = RefFrameTransformCenter.GetLocation();
    FVector SphereTraceEnd = SphereTraceStart + ((-HoverTraceDirection) * (MaxHeightHover + Offset));
    FQuat SphereCheckRotator = FQuat();

    bContactPointActive = UpdatedMesh->GetWorld()->SweepSingleByChannel(SphereTraceOutHit, SphereTraceStart, SphereTraceEnd, SphereCheckRotator, ECC_GameTraceChannel2,
                                                                        SphereCheckShape, SphereTraceQueryParameters, SphereTraceResponseParameters);

    if (bContactPointActive)
    {
        STLocation = SphereTraceOutHit.Location;
        STImpactPoint = SphereTraceOutHit.ImpactPoint;
        STImpactNormal = SphereTraceOutHit.ImpactNormal;
        STPhysMaterial = SphereTraceOutHit.PhysMaterial.Get();
        HoverTraceDirection = STImpactNormal.GetSafeNormal();

        if (bDebugMode)
        {
            DrawDebugSphereTrace(bContactPointActive, SphereTraceStart, SphereTraceEnd, STLocation, STRadius, 1.f, SphereTraceOutHit.ImpactPoint, UpdatedMesh->GetWorld());
        }
    }
    else
    {
        STLocation = FVector::ZeroVector;
        STImpactPoint = FVector::ZeroVector;
        STImpactNormal = FVector::ZeroVector;
        STPhysMaterial = nullptr;
        HoverTraceDirection = FVector::UpVector;
    }
}

void UP4GliderMovementComponent::CalcForceHover(const float DT, FVector& FinForce)
{
    CurrentHeight = (RefFrameTransformCenter.GetLocation() - STLocation).Size() + STRadius - Offset;

    FinForce = UP4FunctionLibrary::Stabilizer(DT, CurrentHeight, MinHeightHover, MaxHeightHover, Force, ForceMultiplier, Damping, PreviousHeight) * HoverTraceDirection;

    PreviousHeight = CurrentHeight;

    // Debug
    if (bDebugMode)
    {
        DrawDebugString(UpdatedMesh->GetWorld(), RefFrameTransformCenter.GetLocation(), FString::SanitizeFloat(CurrentHeight), 0, FColor::White, 0.0f, false);
    }
    // GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("lForce"));
}

void UP4GliderMovementComponent::CalcForceAirDrag(const float DT, FVector& FinForce)
{
    FinForce = UP4FunctionLibrary::CalculateAirDragForce(UpdatedMesh, AirDragCoef, AreaOfDrag, AirDensity);

    // Debug
    if (bDebugMode)
    {
        DrawDebugLine(UpdatedMesh->GetWorld(), RefFrameTransformCenter.GetLocation(), RefFrameTransformCenter.GetLocation() + (FinForce * 0.001), FColor::Orange, true, 1.f, 00,
                      10.f);
    }
}

void UP4GliderMovementComponent::CalcForceFriction(const float DT, FVector& FinForce)
{
    FinForce = UP4FunctionLibrary::CalculateFrictionForce(UpdatedMesh, DT, STImpactNormal, MuCoefficient);

    // Debug
    if (bDebugMode)
    {
        DrawDebugLine(UpdatedMesh->GetWorld(), STImpactPoint, STImpactPoint + (FinForce * 0.001), FColor::Black, true, 1.f, 00, 10.f);
    }
}

void UP4GliderMovementComponent::DrawDebugSphereTrace(bool bBlockingHit, FVector TraceStart, FVector TraceEnd, FVector HitTraceLocation, float SphereRadius, float Thickness,
                                                      FVector HitPoint, UWorld* WorldRef)
{
    FVector CapsuleDirection = TraceEnd - TraceStart;
    FQuat CapsuleRot = FRotationMatrix::MakeFromZ(CapsuleDirection).ToQuat();

    if (bBlockingHit)
    {
        CapsuleDirection = HitTraceLocation - TraceStart;
        float CapsuleLenght = CapsuleDirection.Size();
        FVector CapsuleCenter = TraceStart + CapsuleDirection * 0.5;
        float CapsuleHalfHeight = CapsuleLenght * 0.5f + SphereRadius;
        DrawDebugCapsule(WorldRef, CapsuleCenter, CapsuleHalfHeight, SphereRadius, CapsuleRot, FColor::Red, false, 0.0f, 1, Thickness);

        CapsuleDirection = TraceEnd - HitTraceLocation;
        CapsuleLenght = CapsuleDirection.Size();
        CapsuleCenter = HitTraceLocation + CapsuleDirection * 0.5;
        CapsuleHalfHeight = CapsuleLenght * 0.5f + SphereRadius;
        DrawDebugCapsule(WorldRef, CapsuleCenter, CapsuleHalfHeight, SphereRadius, CapsuleRot, FColor::Green, false, 0.0f, 1, Thickness);

        DrawDebugPoint(WorldRef, HitPoint, 10.0, FColor::Blue, false, 0.0f, 0);
    }
    else
    {
        float CapsuleLenght = CapsuleDirection.Size();
        FVector CapsuleCenter = TraceStart + CapsuleDirection * 0.5;
        float CapsuleHalfHeight = CapsuleLenght * 0.5f + SphereRadius;
        DrawDebugCapsule(WorldRef, CapsuleCenter, CapsuleHalfHeight, SphereRadius, CapsuleRot, FColor::Yellow, false, 0.0f, 1, Thickness);
    }
}
