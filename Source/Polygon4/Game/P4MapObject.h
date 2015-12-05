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
#include <Polygon4/DataManager/Types.h>
#include "P4MapObject.generated.h"

class P4MapObject;

UCLASS()
class POLYGON4_API AP4Object : public AActor
{
	GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = Mesh, meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* VisibleComponent;

public:
    AP4Object();

    virtual void BeginPlay() override;

    void SetStaticMesh(UStaticMesh *mesh);
    void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
    P4MapObject *MapObject;

    friend class P4MapObject;
};

class P4MapObject : public polygon4::detail::MapObject
{
    using Base = polygon4::detail::MapObject;

public:
    P4MapObject(const polygon4::detail::MapObject &rhs);

    virtual bool spawn() override;

private:
    AP4Object *Object;
};
