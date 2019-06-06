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

#include "Projectile.h"

#include "P4Glider.h"

AProjectile::AProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(10.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0;

	// Die after 3 seconds by default
	InitialLifeSpan = 15.0f;

    bCanBeDamaged = false;
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor != this &&
        OtherComp != nullptr &&
        OtherActor != Owner &&
        (OtherComp->IsSimulatingPhysics() || OtherComp->IsWorldGeometry()))
	{
        //if (OtherComp->IsSimulatingPhysics())
		//    OtherComp->AddImpulseAtLocation(GetVelocity() * Impulse, GetActorLocation());
        if (auto Glider = Cast<AP4Glider>(OtherActor))
        {
            if (auto m = Glider->GetMechanoid())
            {
                m->configuration->hit(Projectile);
                if (m->configuration->isDead())
                {
                    Glider->Destroy();
                    // TODO: spawn mechanoid here, show explosion etc.
                }
                else if (m->isPlayer())
                {
                    Glider->FireLightOn(); // for tests
                }
            }
        }
    }
    if (OtherActor != this)
        Destroy();
}
