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

#include "P4MapObject.h"

#include "P4Engine.h"

#include "Landscape.h"
#include "EngineUtils.h"

AP4Object::AP4Object()
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

void AP4Object::BeginPlay()
{
    if (MapObject && MapObject->interactive)
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

void AP4Object::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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
    Object = ::spawn(this, GP4Engine()->GetWorld());
    if (Object)
        Object->MapObject = this;
    return !!Object;
}

AP4Object *spawn(polygon4::detail::MapObject *O, UWorld *W)
{
    auto World = W;
    TActorIterator<ALandscape> landscapeIterator(World);
    ALandscape* landscape = *landscapeIterator;
    auto WorldScale = landscape->GetActorScale() / 100.0f;

    //FVector pos(O->x * 10 * WorldScale.X + O->map->bx, O->y * 10 * WorldScale.Y + O->map->by, O->z * 10);
    FVector pos(O->x, O->y, O->z);
    FRotator rot(O->pitch + O->object->pitch, O->yaw + O->object->yaw, O->roll + O->object->roll);

    if (!O->object->resource.empty())
    {
        auto o = LoadObject<UStaticMesh>(0, O->object->resource);
        if (o)
        {
            auto Object = World->SpawnActorDeferred<AP4Object>(AP4Object::StaticClass(), {});
            UGameplayStatics::FinishSpawningActor(Object, { rot, pos });

            Object->SetStaticMesh(o);
            // respect:
            // 1. global scale (O->object->scale)
            // 2. global coordinate scale (O->object->scale_N)
            // 3. local scale (O->scale)
            // 4. local coordinate scale (O->scale_N)
            FVector new_scale(
                O->scale * O->scale_x * O->object->scale * O->object->scale_x,
                O->scale * O->scale_y * O->object->scale * O->object->scale_y,
                O->scale * O->scale_z * O->object->scale * O->object->scale_z);
            Object->SetActorScale3D(new_scale);
#if WITH_EDITOR
            Object->SetFolderPath("Objects");
            Object->SetActorLabel(O->object->getName());
#endif
            return Object;
        }
    }

    auto c = LoadClass(0);
    auto a = World->SpawnActor<AActor>(c, pos, rot);
#if WITH_EDITOR
    a->SetFolderPath("Objects");
    a->SetActorLabel(O->object->getName());
#endif
    return nullptr;
}
