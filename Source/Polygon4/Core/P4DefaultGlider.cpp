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

#include "P4DefaultGlider.h"

#include "Components/StaticMeshComponent.h"
#include "DisplayDebugHelpers.h"
#include "Engine/CollisionProfile.h"
#include "P4GliderMovementComponent.h"

AP4DefaultGlider::AP4DefaultGlider(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    bReplicateMovement = true;
    NetUpdateFrequency = 10.f;

    GliderPhysicsBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GliderPhysicsBody"));
    GliderPhysicsBody->SetCollisionProfileName(TEXT("GliderPhysicsBody"));
    GliderPhysicsBody->BodyInstance.bSimulatePhysics = true;
    GliderPhysicsBody->BodyInstance.bNotifyRigidBodyCollision = true;
    GliderPhysicsBody->BodyInstance.bUseCCD = true;
    GliderPhysicsBody->SetMassOverrideInKg(NAME_None, 1000.f);
    GliderPhysicsBody->SetLinearDamping(0.f);
    GliderPhysicsBody->SetAngularDamping(0.f);
    GliderPhysicsBody->ScaleByMomentOfInertia(FVector(0.f, 0.f, 0.f));
    GliderPhysicsBody->SetGenerateOverlapEvents(true);
    GliderPhysicsBody->SetCanEverAffectNavigation(false);
    RootComponent = GliderPhysicsBody;

    GliderMovementComponent = CreateDefaultSubobject<UP4GliderMovementComponent>(TEXT("GliderMovementComponent"));
    GliderMovementComponent->UpdatedComponent = RootComponent;
}

// Called when the game starts or when spawned
void AP4DefaultGlider::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AP4DefaultGlider::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AP4DefaultGlider::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
