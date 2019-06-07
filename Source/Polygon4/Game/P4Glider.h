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

#include <queue>

#include "GameFramework/Pawn.h"
#include <Polygon4/DataManager/Types.h>
#include "GliderMovement.h"
#include "P4Glider.generated.h"

USTRUCT()
struct FGliderData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere)
    FString TextID;

    //UPROPERTY(EditAnywhere)
    //TArray<FString> Clan;
};

struct ArmedTimedValue
{
    float max;
    float current = 0;
    bool ready = true;

    ArmedTimedValue(float cooldown = 3)
    {
        max = cooldown;
    }

    ArmedTimedValue& operator+=(float tick)
    {
        if (!ready)
        {
            current += tick;
            ready = current >= max;
            if (ready)
                current = 0;
        }
        return *this;
    }

    operator bool()
    {
        if (ready)
        {
            ready = false;
            return true;
        }
        return false;
    }
};

struct PreparedTimedValue
{
    enum State
    {
        Inactive,
        Arming,
        Ready,
        Rearming,
        Disarming,
    };

    float arm_time;
    float ready_time;
    float disarm_time;
    float current = 0;
    State state;

    PreparedTimedValue(float arm_time = 3, float ready_time = 0, float disarm_time = 0)
    {
        this->arm_time = arm_time;
        if (ready_time == 0)
            ready_time = arm_time * 3;
        this->ready_time = ready_time;
        if (disarm_time == 0)
            disarm_time = arm_time;
        this->disarm_time = disarm_time;
        state = Inactive;
    }

    PreparedTimedValue& operator+=(float tick)
    {
        if (state == Inactive)
            return *this;
        if (state == Arming)
        {
            current += tick;
            if (current > arm_time)
            {
                current = 0;
                state = Ready;
            }
            return *this;
        }
        if (state == Ready)
        {
            current += tick;
            if (current > ready_time)
            {
                current = 0;
                state = Disarming;
            }
            return *this;
        }
        if (state == Rearming)
        {
            current += tick;
            if (current > arm_time)
            {
                current = 0;
                state = Ready;
            }
            return *this;
        }
        if (state == Disarming)
        {
            current += tick;
            if (current > disarm_time)
            {
                current = 0;
                state = Inactive;
            }
            return *this;
        }
        return *this;
    }

    operator bool()
    {
        if (state == Inactive)
        {
            state = Arming;
            return false;
        }
        if (state == Ready)
        {
            state = Rearming;
            current = 0;
            return true;
        }
        if (state == Disarming)
            return false;
        return false;
    }
};

template <typename T>
struct FadedValue
{
    T time;
    T fade_time;
    bool started;

    FadedValue(T s = 2)
        : fade_time(s)
    {
        reset();
    }

    void reset()
    {
        time = T();
        started = false;
    }

    void start()
    {
        reset();
        started = true;
    }

    FadedValue& operator=(T f)
    {
        return plus(f);
    }
    FadedValue& operator+(T f)
    {
        return plus(f);
    }
    FadedValue& operator+=(T f)
    {
        return plus(f);
    }

    FadedValue& plus(T f)
    {
        time += f;
        return *this;
    }

    operator bool()
    {
        if (started)
        {
            if (time <= fade_time)
                return true;
            else
                reset();
        }
        return false;
    }
};

template <typename T>
class DampingValue
{
public:
    DampingValue(size_t n = 5)
        : n(n)
    {
    }

    DampingValue& operator=(T f)
    {
        return plus(f);
    }
    DampingValue& operator+(T f)
    {
        return plus(f);
    }
    DampingValue& operator+=(T f)
    {
        return plus(f);
    }

    DampingValue& plus(T f)
    {
        values.push_back(f);
        if (values.size() > n)
            values.pop_front();
        return *this;
    }

    operator T() const
    {
        if (values.empty())
            return T();
        T sum = T();
        for (auto &v : values)
            sum = sum + v;
        return sum / n;
    }

private:
    std::deque<T> values;
    size_t n;
};

using FloatDampingValue = DampingValue<float>;
using FloatFadedValue = FadedValue<float>;

USTRUCT()
struct FPowerUpProperties
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "PowerupClass")
    bool HoverInAir;

    UPROPERTY(EditDefaultsOnly, Category = "PowerupClass")
    float HoverAltitude;

    UPROPERTY(EditDefaultsOnly, Category = "PowerupClass")
    float GravityScale;

    UPROPERTY(EditDefaultsOnly, Category = "PowerupClass")
    float LiftSpring;

    UPROPERTY(EditDefaultsOnly, Category = "PowerupClass")
    float LiftDamp;

    UPROPERTY(EditDefaultsOnly, Category = "PowerupClass")
    bool SlowFall;

    UPROPERTY(EditDefaultsOnly, Category = "PowerupClass")
    float FallSpeed;

    UPROPERTY(EditDefaultsOnly, Category = "PowerupClass")
    bool SelfRight;

    /* Speed That It Rights Itself */
    UPROPERTY(EditDefaultsOnly, Category = "PowerupClass")
    float AlphaTrack;

    /* Self-Right Damping */
    UPROPERTY(EditDefaultsOnly, Category = "PowerupClass")
    float AlphaDamp;

    FPowerUpProperties();
};

UCLASS()
class POLYGON4_API AP4Glider : public APawn
{
    enum EGliderView
    {
        FPS,
        //FPSWeapons,
        TPS,

        Max
    };

    GENERATED_BODY()

    /** First person camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FirstPersonCameraComponent;

    /** Third person camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* ThirdPersonCameraComponent;

    /** Spring arm for TPCamera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, Category = Mesh, meta = (AllowPrivateAccess = "true"))
    class UStaticMeshComponent* VisibleComponent;

    UPROPERTY(VisibleAnywhere, Category = Mesh, meta = (AllowPrivateAccess = "true"))
    class UStaticMeshComponent* EnergyShield;

    UGliderMovement *GliderMovement;

    // spot light

    int GliderView = EGliderView::FPS;

public:
    UPROPERTY(EditAnywhere, Category = Mechanoid)
    FGliderData Data;

public:
    // Sets default values for this pawn's properties
    AP4Glider();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* InInputComponent) override;

    void BoostOn() { boost = true; }
    void BoostOff() { boost = false; }
    void FireLightOn() { FireLight = true; }
    void FireLightOff() { FireLight = false; }
    void FireHeavyOn() { FireHeavy = true; }
    void FireHeavyOff() { FireHeavy = false; }

    virtual UGliderMovement *GetMovementComponent() const override;

private:
    bool LeftGun = true;
    UClass *projectileLight;
    UClass *projectileHeavy;

    FVector GunOffsetLeft;
    FVector GunOffsetRight;
    FVector GunOffsetTop;
    FVector GunOffsetTopLeft;
    FVector GunOffsetTopRight;

	class UPrimitiveComponent* Body;
	class USoundWave* JumpSound = nullptr;
	class USoundWave* EngineSound = nullptr;
	class USoundWave* LightSound = nullptr;
	class USoundWave* HeavySound = nullptr;
	class UAudioComponent* EngineAudioComponent;
	class UAudioComponent* WeaponAudioComponent;

    FloatFadedValue EnergyShieldTimer;

    // functions
    void Move(float AxisValue);
    void Strafe(float AxisValue);

    void Turn(float AxisValue);
    void LookUp(float AxisValue);

    void ChangeView();
    void UpdateView();

    ArmedTimedValue JumpTimeout;
    void Jump();

    void HideUI();

private:
    bool boost = false;
    bool FireLight = false;
    bool FireHeavy = false;

    float Lifetime = 0.0f;

private:
    FPowerUpProperties PowerUpProperties;
    FHitResult ZTraceResults;

    FVector2D MousePosition;
    int MousePositionRepeats = 0;

    FloatDampingValue SlopeAngle;

    bool UIHidden = false;

    // Vertical
    FHitResult HoverTrace(float Altitude = 50000.0f) const;
    // To Point
    FHitResult HoverTrace(FVector Vector, float Altitude = 50000.0f) const;

private:
    polygon4::detail::Mechanoid* Mechanoid = nullptr;
    polygon4::detail::Configuration* Configuration = nullptr;
    polygon4::detail::Glider* Glider = nullptr;

public:
    void SetMechanoid(polygon4::detail::Mechanoid* Mechanoid);
    polygon4::detail::Mechanoid* GetMechanoid() const { return Mechanoid; }

    float GetLifetime() const { return Lifetime; }
    bool CanEnterBuilding() const { return GetLifetime() > 3.0f; }

    void SetStaticMesh(UStaticMesh *mesh);

public:
    UFUNCTION()
    void OnBodyHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
    UFUNCTION()
    void OnBodyBeginOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnEnergyShieldBeginOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void OnEnergyShieldEndOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
