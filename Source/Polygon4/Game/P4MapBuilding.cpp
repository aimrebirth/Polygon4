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

#include "P4MapBuilding.h"

#include "Landscape.h"

#include "P4Engine.h"
#include "P4Glider.h"

#include "EngineUtils.h"
#include "Landscape.h"

AP4Building::AP4Building()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleComponent"));
    VisibleComponent->SetupAttachment(RootComponent);

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

void AP4Building::OnBodyHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    OnHit(OtherActor, OtherComp);
}

void AP4Building::OnBodyBeginOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
    if (!M || !Glider->CanEnterBuilding())
        return;
    Glider->Destroy();
    M->enterBuilding(MapBuilding);
}

P4MapBuilding::P4MapBuilding(const polygon4::detail::MapBuilding &rhs)
    : Base(rhs)
{
}

bool P4MapBuilding::spawn()
{
    Building = ::spawn(this, GP4Engine()->GetWorld());
    if (Building)
        Building->MapBuilding = this;
    return !!Building;
}

void P4MapBuilding::initModificationMapBuilding()
{
    if (Building)
        Building->InitModificationMapBuilding(modificationMapBuilding);
}

AP4Building *spawn(polygon4::detail::MapBuilding *B, UWorld *W)
{
    auto World = W;
    TActorIterator<ALandscape> landscapeIterator(World);
    ALandscape* landscape = *landscapeIterator;
    auto WorldScale = landscape->GetActorScale() / 100.0f;

    FVector pos(B->x * 10 * WorldScale.X + B->map->bx, B->y * 10 * WorldScale.Y + B->map->by, B->z * 10);
    FRotator rot(B->pitch + B->building->pitch, B->yaw + B->building->yaw, B->roll + B->building->roll);

    if (!B->building->resource.empty())
    {
        auto o = LoadObject<UStaticMesh>(0, B->building->resource);
        if (o)
        {
            auto Building = World->SpawnActorDeferred<AP4Building>(AP4Building::StaticClass(), {});
            UGameplayStatics::FinishSpawningActor(Building, { rot, pos });

            Building->SetStaticMesh(o);
            FVector new_scale(B->building->scale, B->building->scale, B->building->scale);
            Building->SetActorScale3D(new_scale);
#if WITH_EDITOR
            Building->SetFolderPath("Buildings");
            Building->SetActorLabel(B->building->getName());
#endif
            return Building;
        }
    }

    auto c = LoadClass(0);
    auto a = World->SpawnActor<AActor>(c, pos, rot);
#if WITH_EDITOR
    a->SetFolderPath("Buildings");
    a->SetActorLabel(B->building->getName());
#endif
    return nullptr;
}
