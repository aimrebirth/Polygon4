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

#include "Polygon4.h"
#include "P4MapObject.h"

#include "P4Engine.h"

AP4Object::AP4Object()
{
	PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleComponent"));
    VisibleComponent->AttachTo(RootComponent);

#if WITH_EDITOR
    RootComponent->SetMobility(EComponentMobility::Movable);
#else
    RootComponent->SetMobility(EComponentMobility::Stationary);
#endif
}

void AP4Object::BeginPlay()
{
    if (MapObject->interactive)
        VisibleComponent->OnComponentHit.AddDynamic(this, &AP4Object::OnHit);

    Super::BeginPlay();
}

void AP4Object::SetStaticMesh(UStaticMesh *mesh)
{
    if (mesh)
    {
        VisibleComponent->SetStaticMesh(mesh);
    }
}

void AP4Object::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor != this && OtherComp != NULL)
    {
    }
}

P4MapObject::P4MapObject(const polygon4::detail::MapObject &rhs)
    : Base(rhs)
{
}

bool P4MapObject::spawn()
{
    auto World = GP4Engine->GetWorld();
    auto WorldScale = GP4Engine->GetWorldScale();

    FVector pos(x * 10 * WorldScale.X + map->bx, y * 10 * WorldScale.Y + map->by, z * 10);
    FRotator rot(pitch, yaw, roll);

    if (!object->resource.empty())
    {
        auto o = LoadObject<UStaticMesh>(0, object->resource);
        if (o)
        {
            Object = World->SpawnActor<AP4Object>(AP4Object::StaticClass(), pos, rot);
            Object->MapObject = this;
            Object->SetStaticMesh(o);
            FVector scale(object->scale, object->scale, object->scale);
            Object->SetActorScale3D(scale);
#if WITH_EDITOR
            Object->SetActorLabel(object->getName());
#endif
            return true;
        }
    }

    auto c = LoadClass(0);
    auto a = World->SpawnActor<AActor>(c, pos, rot);
#if WITH_EDITOR
    a->SetActorLabel(object->getName());
#endif
    return true;
}
