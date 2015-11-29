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

#include "GameFramework/Pawn.h"
#include <Polygon4/DataManager/Types.h>
#include "P4Glider.generated.h"

class UGliderMovement;

USTRUCT()
struct FGliderData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere)
    FString TextID;

    //UPROPERTY(EditAnywhere)
    //TArray<FString> Clan;
};

enum EGliderView
{
    FPS,
    //FPSWeapons,
    TPS,

    Max
};

struct ArmedTimedValue
{
    float max;
    float current = 0;
    bool ready = true;

    ArmedTimedValue(float cooldown = 3)
    {
        this->max = cooldown;
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
    UStaticMeshComponent* VisibleComponent;
    UStaticMeshComponent* Mesh;
    
    //UPROPERTY()
    //UGliderMovement* MovementComponent;

    int GliderView = EGliderView::FPS;

public:
    UPROPERTY(EditAnywhere, Category = Mechanoid)
    FGliderData Data;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

public:
	// Sets default values for this pawn's properties
	AP4Glider();

	virtual void BeginPlay() override;	
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InInputComponent) override;
    //virtual UPawnMovementComponent* GetMovementComponent() const override;

private:
    // data
    float RunningTime;
    float time1 = 0.0f;
    float time2 = 0.0f;
    bool LeftGun = true;

    ArmedTimedValue JumpTimeout;

    UClass *b1 = 0;
    UClass *b2 = 0;

    FVector CurrentVelocity;
    bool boost = false;

    bool FireLight = false;
    bool FireHeavy = false;

    // functions
    void Move(float AxisValue);
    void Strafe(float AxisValue);

    void Turn(float AxisValue);
    void LookUp(float AxisValue);

    void ChangeView();
    void UpdateView();

    void Jump();

    void BoostOn();
    void BoostOff();

    void FireLightOn();
    void FireLightOff();
    void FireHeavyOn();
    void FireHeavyOff();

public:

private:
    UAudioComponent *JumpSound;

private:
    FPowerUpProperties PowerUpProperties;
    FHitResult ZTraceResults;

    void Hover();
    FHitResult HoverTrace(float Altitude) const;
};
