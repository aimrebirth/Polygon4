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
#include "Glider.h"

#include "GliderMovement.h"

AGlider::AGlider()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    // Set this pawn to be controlled by the lowest-numbered player
    AutoPossessPlayer = EAutoReceiveInput::Player0;

    // Create a dummy root component we can attach things to.
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    MovementComponent = CreateDefaultSubobject<UGliderMovement>(TEXT("CustomMovementComponent"));
    MovementComponent->UpdatedComponent = RootComponent;
    	
    VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleComponent"));
    
    static ConstructorHelpers::FObjectFinder<UStaticMesh> GliderMesh(TEXT("/Game/Mods/Common/Gliders/Hawk/GLD_M1_BASE"));
    if (GliderMesh.Succeeded())
    {
        VisibleComponent->SetStaticMesh(GliderMesh.Object);
        VisibleComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
    }
    VisibleComponent->AttachTo(RootComponent);

    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
    FirstPersonCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    FirstPersonCameraComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    FirstPersonCameraComponent->bUsePawnControlRotation = true;
    FirstPersonCameraComponent->AttachTo(RootComponent);

    ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TPSCamera"));
    ThirdPersonCameraComponent->SetRelativeLocation(FVector(-100.0f, 0.0f, 50.0f));
    ThirdPersonCameraComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    ThirdPersonCameraComponent->bUsePawnControlRotation = false;
    ThirdPersonCameraComponent->AttachTo(RootComponent);

    // set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
    
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    UpdateView();
}

// Called when the game starts or when spawned
void AGlider::BeginPlay()
{
	Super::BeginPlay();
	
}

/*UPawnMovementComponent* AGlider::GetMovementComponent() const
{
    return MovementComponent;
}*/

// Called every frame
void AGlider::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
    
    /*FVector NewLocation = GetActorLocation();
    float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
    NewLocation.Z += DeltaHeight * 1.0f;       //Scale our height by a factor of 20
    RunningTime += DeltaTime;
    SetActorLocation(NewLocation);

    // move
    if (!CurrentVelocity.IsZero())
    {
        FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
        SetActorLocation(NewLocation);
    }*/
}

// Called to bind functionality to input
void AGlider::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
    
    InputComponent->BindAction("Exit", IE_Pressed, this, &AGlider::ShowMenu);
    InputComponent->BindAction("Pause", IE_Pressed, this, &AGlider::ShowMenu);
    InputComponent->BindAction("View", IE_Pressed, this, &AGlider::ChangeView);

    InputComponent->BindAxis("Move", this, &AGlider::Move);
    InputComponent->BindAxis("Strafe", this, &AGlider::Strafe);
    
    //InputComponent->BindAxis("Turn", this, &AGlider::Turn);
	//InputComponent->BindAxis("LookUp", this, &AGlider::LookUp);
    InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

    InputComponent->BindAction("Jump", IE_Pressed, this, &AGlider::Jump);
    //InputComponent->BindAction("Jump", IE_Released, this, &AGlider::Jump);
    
    InputComponent->BindAction("Boost", IE_Pressed, this, &AGlider::BoostOn);
    InputComponent->BindAction("Boost", IE_Released, this, &AGlider::BoostOff);
}

void AGlider::Move(float AxisValue)
{
    if (Controller != NULL && AxisValue != 0.0f)
    {
        FRotator Rotation = Controller->GetControlRotation();
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
        AddMovementInput(Direction, AxisValue);
    }
    if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
    {
        //MovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
    }
}

void AGlider::Strafe(float AxisValue)
{
    if (Controller != NULL && AxisValue != 0.0f)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
        AddMovementInput(Direction, AxisValue);
    }
    if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
    {
        //MovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
    }
}

void AGlider::Jump()
{
}

void AGlider::ShowMenu()
{
}

void AGlider::BoostOn()
{
    boost = true;
}

void AGlider::BoostOff()
{
    boost = false;
}
    
void AGlider::Turn(float AxisValue)
{
    FRotator NewRotation = GetActorRotation();
    NewRotation.Yaw += AxisValue;
    SetActorRotation(NewRotation);
}

void AGlider::LookUp(float AxisValue)
{
    FRotator NewRotation = GetActorRotation();
    NewRotation.Pitch += AxisValue;
    SetActorRotation(NewRotation);
}

void AGlider::ChangeView()
{
    GliderView = (GliderView + 1) % EGliderView::Max;
    UpdateView();
}

void AGlider::UpdateView()
{
    switch (GliderView)
    {
    case FPS:
    case FPSWeapons:
        VisibleComponent->SetOwnerNoSee(true);
        break;
    case TPS:
        VisibleComponent->SetOwnerNoSee(false);
        break;
    }
}