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

#pragma once

#include "GameFramework/PawnMovementComponent.h"

#include "PhysicsEngine/BodyInstance.h"
#include "Public/CollisionQueryParams.h"
#include "Public/WorldCollision.h"

#include "P4GliderMovementComponent.generated.h"

UCLASS(Blueprintable, BlueprintType)
class POLYGON4_API UP4GliderMovementComponent : public UPawnMovementComponent
{
    GENERATED_UCLASS_BODY()

protected:
    //////////////////////////////////////////////////////////////////////////
    // Initialization

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    //////////////////////////////////////////////////////////////////////////
    // Physics initialization

    void InitMesh();
    void InitBodyPhysics();

public:
    /** The mesh we move and update */
    UPROPERTY(BlueprintReadOnly, Transient, DuplicateTransient)
    UStaticMeshComponent* UpdatedMesh;

    // PUBLIC //

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glider Movement Component")
    bool bIsEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glider Movement Component")
    bool bDebugMode = false;

    // Hover //
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glider Movement Component|Hover",
              meta = (EditCondition = "bIsEnabled", ToolTip = "The socket name from which the trace begins."))
    FName HoverTraceStart = "HoverTraceStart";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glider Movement Component|Hover",
              meta = (EditCondition = "bIsEnabled", ToolTip = "The minimum height to which the maximum force will be applied"))
    float MinHeightHover = 70.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glider Movement Component|Hover",
              meta = (EditCondition = "bIsEnabled", ToolTip = "The maximum height of which will adhere to the algorithm"))
    float MaxHeightHover = 85.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glider Movement Component|Hover",
              meta = (EditCondition = "bIsEnabled", ToolTip = "Automatic calculating min force F = m * a"))
    bool bIsCalcForce = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glider Movement Component|Hover", meta = (EditCondition = "!bIsCalcForce", ToolTip = "Force in the Newton"))
    float Force = 10000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glider Movement Component|Hover",
              meta = (EditCondition = "bIsEnabled", ToolTip = "As spring compresses it produces more and more force which can push vehicle more than desired"))
    float ForceMultiplier = 3.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glider Movement Component|Hover",
              meta = (EditCondition = "bIsEnabled", ToolTip = "Dampening coefficient of the spring"))
    float Damping = 50.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glider Movement Component|Hover",
              meta = (EditCondition = "bIsEnabled", ToolTip = "Radius of the Sphere Collision Trace in cm"))
    float STRadius = 35.f;
    // Hover //

    // AirDrag //
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glider Movement Component|AirDrag", meta = (ToolTip = "Axis air drag coefficients"))
    FVector AirDragCoef = FVector(0.5, 1.f, 1.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glider Movement Component|AirDrag", meta = (ToolTip = "Area of drag, in centimeters square"))
    float AreaOfDrag = 125.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glider Movement Component|AirDrag", meta = (ToolTip = "Air density at a temperature of 10 degrees"))
    float AirDensity = 1.1666;
    // AirDrag //

    // Friction //
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glider Movement Component|Friction",
              meta = (EditCondition = "bIsEnabled", ToolTip = "The thickness of the collision lines Sphere Collision Trace"))
    bool bIsFriction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glider Movement Component|Friction",
              meta = (EditCondition = "bIsFriction", ToolTip = "Coefficient of frictional force"))
    float MuCoefficient = 1.0f;
    // Friction //

    // Public Fucntions

    UFUNCTION(BlueprintPure, Category = "Glider Movement Component")
    FVector GetHoverTraceDirection()
    {
        return HoverTraceDirection;
    };

    UFUNCTION(BlueprintPure, Category = "Glider Movement Component")
    bool GetContactPointActive()
    {
        return bContactPointActive;
    };

    UFUNCTION(BlueprintPure, Category = "Glider Movement Component")
    FVector GetSTLocation()
    {
        return STLocation;
    };

    UFUNCTION(BlueprintPure, Category = "Glider Movement Component")
    FVector GetSTImpactPoint()
    {
        return STImpactPoint;
    };

    UFUNCTION(BlueprintPure, Category = "Glider Movement Component")
    FVector GetSTImpactNormal()
    {
        return STImpactNormal;
    };

    UFUNCTION(BlueprintPure, Category = "Glider Movement Component")
    UPhysicalMaterial* GetSTPhysMaterial()
    {
        return STPhysMaterial;
    };

    UFUNCTION(BlueprintPure, Category = "Glider Movement Component")
    float GetCurrentHeight()
    {
        return CurrentHeight;
    };

    UFUNCTION(BlueprintPure, Category = "Glider Movement Component")
    float GetPreviousHeight()
    {
        return PreviousHeight;
    };

    UFUNCTION(BlueprintPure, Category = "Glider Movement Component")
    float GetOffset()
    {
        return Offset;
    };

    // Public Fucntions

private:
    UPROPERTY()
    bool bItsChecksOk;
    UPROPERTY()
    FTransform RefFrameTransformCenter;
    UPROPERTY()
    bool bContactPointActive;
    UPROPERTY()
    FVector STLocation;
    UPROPERTY()
    FVector STImpactPoint;
    UPROPERTY()
    FVector STImpactNormal;
    UPROPERTY()
    UPhysicalMaterial* STPhysMaterial;
    UPROPERTY()
    FVector HoverTraceDirection;
    UPROPERTY()
    float Offset;
    UPROPERTY()
    float CurrentHeight;
    UPROPERTY()
    float PreviousHeight;

    FCollisionQueryParams SphereTraceQueryParameters;
    FCollisionResponseParams SphereTraceResponseParameters;
    FCollisionShape SphereCheckShape;

    void UpdatePhysics(const float DT);

    void STCheckGround();

    void CalcForceHover(const float DT, FVector& FinForce);

    void CalcForceAirDrag(const float DT, FVector& FinForce);

    void CalcForceFriction(const float DT, FVector& FinForce);

    static void DrawDebugSphereTrace(bool bBlockingHit, FVector SphereStart, FVector SphereEnd, FVector SphereCenter, float SphereRadius, float Thickness, FVector HitPoint,
                                     UWorld* WorldRef);
};
