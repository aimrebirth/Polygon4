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
#include "Projectile.generated.h"

namespace polygon4
{
namespace detail
{
class Projectile;
}
} // namespace polygon4

UCLASS(Abstract, Blueprintable)
class POLYGON4_API AProjectile : public AActor
{
    GENERATED_BODY()

    UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
    class USphereComponent* CollisionComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    class UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(EditAnywhere, Category = Power, meta = (AllowPrivateAccess = "true"))
    float Impulse = 100.0f;

public:
    AProjectile(const FObjectInitializer& ObjectInitializer);

    virtual void SetOwner(AActor* OtherOwner)
    {
        Owner = OtherOwner;
    }

    void SetProjectile(polygon4::detail::Projectile* InProjectile)
    {
        Projectile = InProjectile;
    }

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    FORCEINLINE class USphereComponent* GetCollisionComp() const
    {
        return CollisionComp;
    }
    FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const
    {
        return ProjectileMovement;
    }

private:
    AActor* Owner = nullptr;
    polygon4::detail::Projectile* Projectile = nullptr;
};
