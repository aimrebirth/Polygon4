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
#include "P4Glider.h"

AP4Building::AP4Building()
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

void AP4Building::BeginPlay()
{
    Super::BeginPlay();
}

void AP4Building::SetStaticMesh(UStaticMesh *mesh)
{
    if (mesh)
    {
        VisibleComponent->SetStaticMesh(mesh);
    }
}

void AP4Building::InitModificationMapBuilding(polygon4::detail::ModificationMapBuilding *mmb)
{
    if (!mmb)
        return;
    if (mmb->interactive)
    {
        VisibleComponent->SetCollisionProfileName("OverlapAllDynamic");
        //VisibleComponent->OnComponentHit.AddDynamic(this, &AP4Building::OnBodyHit);
        VisibleComponent->OnComponentBeginOverlap.AddDynamic(this, &AP4Building::OnBodyBeginOverlap);
    }
}

void AP4Building::OnBodyHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    OnHit(OtherActor, OtherComp);
}

void AP4Building::OnBodyBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    OnHit(OtherActor, OtherComp);
}

void AP4Building::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
    if (OtherActor == nullptr || OtherComp == nullptr || OtherActor == this)
        return;
    auto Glider = Cast<AP4Glider>(OtherActor);
    if (!Glider)
        return;
    auto M = Glider->GetMechanoid();
    Glider->Destroy();
    M->enterBuilding(MapBuilding);
}

P4MapBuilding::P4MapBuilding(const polygon4::detail::MapBuilding &rhs)
    : Base(rhs)
{
}

bool P4MapBuilding::spawn()
{
    auto World = GP4Engine()->GetWorld();
    auto WorldScale = GP4Engine()->GetWorldScale();

    FVector pos(x * 10 * WorldScale.X + map->bx, y * 10 * WorldScale.Y + map->by, z * 10);
    FRotator rot(pitch + building->pitch, yaw + building->yaw, roll + building->roll);

    if (!building->resource.empty())
    {
        auto o = LoadObject<UStaticMesh>(0, building->resource);
        if (o)
        {
            Building = World->SpawnActor<AP4Building>(AP4Building::StaticClass(), pos, rot);
            Building->MapBuilding = this;
            Building->SetStaticMesh(o);
            FVector scale(building->scale, building->scale, building->scale);
            Building->SetActorScale3D(scale);
#if WITH_EDITOR
            Building->SetActorLabel(building->getName());
#endif
            return true;
        }
    }

    auto c = LoadClass(0);
    auto a = World->SpawnActor<AActor>(c, pos, rot);
#if WITH_EDITOR
    a->SetActorLabel(building->getName());
#endif
    return false;
}

void P4MapBuilding::initModificationMapBuilding()
{
    Building->InitModificationMapBuilding(modificationMapBuilding);
}
