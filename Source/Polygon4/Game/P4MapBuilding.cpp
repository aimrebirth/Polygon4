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
#include "P4MapBuilding.h"

#include "P4Engine.h"

AP4Building::AP4Building()
{
	PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleComponent"));
    VisibleComponent->AttachTo(RootComponent);
    
    RootComponent->SetMobility(EComponentMobility::Stationary);
}

void AP4Building::setStaticMesh(UStaticMesh *mesh)
{
    if (mesh)
    {
        VisibleComponent->SetStaticMesh(mesh);
    }
}

P4MapBuilding::P4MapBuilding(const polygon4::detail::MapBuilding &rhs)
    : Base(rhs)
{
}

bool P4MapBuilding::spawn()
{
    auto World = GP4Engine->GetWorld();
    auto WorldScale = GP4Engine->GetWorldScale();
    auto o = LoadObject<UStaticMesh>(0, to_wstring(building->resource).c_str());
    FVector pos(x * 10 * WorldScale.X + map->bx, y * 10 * WorldScale.Y + map->by, z * 10);
    FRotator rot(pitch, yaw, roll);
    if (o)
    {
        Building = World->SpawnActor<AP4Building>(AP4Building::StaticClass(), pos, rot);
        Building->setStaticMesh(o);
        FVector scale(building->scale, building->scale, building->scale);
        Building->SetActorScale3D(scale);
#if WITH_EDITOR
        Building->SetActorLabel(to_wstring(building->getName()).c_str());
#endif
    }
    else
    {
        auto c = LoadClass(0);
        auto a = World->SpawnActor<AActor>(c, pos, rot);
#if WITH_EDITOR
        a->SetActorLabel(to_wstring(building->getName()).c_str());
#endif
    }
    return true;
}