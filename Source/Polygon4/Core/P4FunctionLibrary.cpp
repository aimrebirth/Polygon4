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

#include "P4FunctionLibrary.h"

#include "Components/PrimitiveComponent.h"
#include "Kismet/KismetMathLibrary.h"

UP4FunctionLibrary::UP4FunctionLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

float UP4FunctionLibrary::Stabilizer(const float DT, const float CurrentValue, const float MinValue, const float MaxValue, const float MinForce, const float ForceMultiplier,
                                     const float Damping, const float OldValue)
{
    float lDelta;
    float lComressionRatio;
    float lForce;
    float lDamping;

    lDelta = FMath::Clamp((MaxValue - CurrentValue) / (MaxValue - MinValue), 0.0f, MaxValue);
    lComressionRatio = FMath::Clamp((CurrentValue - 1.0f) / MaxValue, 0.0f, 1.0f);

    if (lDelta > 1.0f)
    {
        lForce = FMath::Lerp(MinForce * ForceMultiplier, MinForce, lComressionRatio);
    }
    else
    {
        lForce = MinForce * lDelta;
    }

    lDamping = FMath::Clamp(Damping * ((CurrentValue - OldValue) / DT), 0.0f, lForce);

    return lForce - lDamping;
}

FVector UP4FunctionLibrary::CalculateAirDragForce(UPrimitiveComponent* Target, const FVector AirDragCoef, const float AreaOfDrag, const float AirDensity)
{
    if (IsValid(Target))
    {
        FTransform lTransform = Target->GetComponentTransform();
        FVector lLinearVelocity = Target->GetPhysicsLinearVelocity();
        FVector lInverseLinearVelocity = UKismetMathLibrary::InverseTransformDirection(lTransform, lLinearVelocity);
        FVector lLinearVelocityABS = lInverseLinearVelocity.GetAbs() * (-lInverseLinearVelocity);

        FVector lAirDragForce = (AirDragCoef * (AreaOfDrag / 100.f) * lLinearVelocityABS * AirDensity) / 2;

        return UKismetMathLibrary::TransformDirection(lTransform, lAirDragForce / 100.f);
    }
    return FVector::ZeroVector;
}

FVector UP4FunctionLibrary::CalculateFrictionForce(UPrimitiveComponent* Target, const float DT, const FVector ImpactNormal, const float MuCoefficient)
{
    if (IsValid(Target))
    {
        float Gravity = 980.f;
        float Mass = Target->GetMass();
        FTransform lTransform = Target->GetComponentTransform();
        FVector LinearVelocity = Target->GetPhysicsLinearVelocityAtPoint(lTransform.GetLocation(), NAME_None);
        FVector ProjectVectorPlane = -FVector::VectorPlaneProject(LinearVelocity, ImpactNormal);
        FVector lFrictionForce = (((ProjectVectorPlane * Mass) / DT) * MuCoefficient);

        lFrictionForce.X = FMath::Clamp(lFrictionForce.X, -Gravity, Gravity);
        lFrictionForce.Y = FMath::Clamp(lFrictionForce.Y, -Gravity, Gravity);
        lFrictionForce.Z = FMath::Clamp(lFrictionForce.Z, -Gravity, Gravity);

        return lFrictionForce;
    }
    return FVector::ZeroVector;
}
