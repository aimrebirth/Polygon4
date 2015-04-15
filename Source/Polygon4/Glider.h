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
#include "Glider.generated.h"

enum EGliderView
{
    FPS,
    FPSWeapons,
    TPS,

    Max
};

UCLASS()
class POLYGON4_API AGlider : public APawn
{
	GENERATED_BODY()
        
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Third person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ThirdPersonCameraComponent;
    
    UPROPERTY()
    UStaticMeshComponent* VisibleComponent;
    
    UPROPERTY()
    class UGliderMovement* MovementComponent;

    int GliderView = EGliderView::FPS;
public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

public:
	// Sets default values for this pawn's properties
	AGlider();

	virtual void BeginPlay() override;	
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
    //virtual UPawnMovementComponent* GetMovementComponent() const override;

private:
    float RunningTime;

    FVector CurrentVelocity;
    bool boost = false;

    void Move(float AxisValue);
    void Strafe(float AxisValue);

    void Turn(float AxisValue);
    void LookUp(float AxisValue);

    void ChangeView();
    void UpdateView();

    void Jump();

    void BoostOn();
    void BoostOff();

    void ShowMenu();
};
