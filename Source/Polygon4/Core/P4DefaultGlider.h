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

#include "GameFramework/Pawn.h"

#include "P4DefaultGlider.generated.h"

/**
 * Glider class with implemented custom physics
 */
UCLASS(abstract, Blueprintable, BlueprintType)
class POLYGON4_API AP4DefaultGlider : public APawn
{
    GENERATED_UCLASS_BODY()
public:
    // Sets default values for this pawn's properties
    AP4DefaultGlider();

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaSeconds) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    //////////////////////////////////////////////////////////////////////////
    // Glider setup

    UPROPERTY(Category = "GliderPhysicsBody", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UStaticMeshComponent* GliderPhysicsBody;

    UPROPERTY(Category = "Glider Movement Component", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UP4GliderMovementComponent* GliderMovementComponent;
};
