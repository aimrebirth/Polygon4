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
#include "P4Glider.generated.h"

enum EGliderView
{
    FPS,
    //FPSWeapons,
    TPS,

    Max
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
    
    UPROPERTY(VisibleAnywhere, Category = Mesh, meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* VisibleComponent;
    
    UPROPERTY()
    class UGliderMovement* MovementComponent;

    int GliderView = EGliderView::FPS;

public:
    UPROPERTY(VisibleAnywhere, Category = Mechanoid)
    FString TextID;

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
};
