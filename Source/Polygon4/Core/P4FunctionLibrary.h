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

#include "Kismet/BlueprintFunctionLibrary.h"
#include "P4FunctionLibrary.generated.h"

UCLASS()
class POLYGON4_API UP4FunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()

public:

    UFUNCTION(BlueprintCallable, meta = (DisplayName = " CalculateAirDragForce "), Category = " Utility")
    static FVector CalculateAirDragForce(UPrimitiveComponent* Target, const FVector AirDragCoef, const float AreaOfDrag, const float AirDensity);

    UFUNCTION(BlueprintCallable, meta = (DisplayName = " CalculateFrictionForce "), Category = " Utility")
    static FVector CalculateFrictionForce(UPrimitiveComponent* Target, const float DT, const FVector ImpactNormal, const float MuCoefficient);

    UFUNCTION(BlueprintCallable, meta = (DisplayName = " Stabilizer "), Category = " Utility")
    static float Stabilizer(const float DT, const float CurrentValue, const float MinValue, const float MaxValue, const float MinForce, const float ForceMultiplier,
                            const float Damping, const float OldValue);

};
