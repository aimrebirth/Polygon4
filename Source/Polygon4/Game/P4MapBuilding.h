/*
 * Polygon-4
 * Copyright (C) 2015 lzwdgc
 *
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

#include "GameFramework/Actor.h"

#include "Windows/WindowsHWrapper.h"
#include <Polygon4/MapBuilding.h>
#include "Windows/WindowsHWrapper.h"

#include "P4MapBuilding.generated.h"

class P4MapBuilding;

UCLASS()
class POLYGON4_API AP4Building : public AActor
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = Mesh, meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* VisibleComponent;

public:
    AP4Building();

    virtual void BeginPlay() override;

    void SetStaticMesh(UStaticMesh *mesh);
    void InitModificationMapBuilding(polygon4::detail::ModificationMapBuilding *mmb);

    UFUNCTION()
    void OnBodyHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
    UFUNCTION()
    void OnBodyBeginOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp);

private:
    P4MapBuilding *MapBuilding = nullptr;

    class USoundWave* Sound = nullptr;

    friend class P4MapBuilding;
};

class P4MapBuilding : public polygon4::MapBuilding
{
    using Base = polygon4::MapBuilding;

public:
    P4MapBuilding(const polygon4::detail::MapBuilding &rhs);

    virtual bool spawn() override final;

    virtual void initModificationMapBuilding() override final;

private:
    AP4Building *Building;
};

POLYGON4_API
AP4Building *spawn(polygon4::detail::MapBuilding *B, UWorld *W);
