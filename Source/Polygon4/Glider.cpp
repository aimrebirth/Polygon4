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
    ThirdPersonCameraComponent->AttachTo(FirstPersonCameraComponent);

    // set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
    
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    UpdateView();

    static ConstructorHelpers::FObjectFinder<UClass> light(TEXT("Class'/Game/Mods/Common/Projectiles/SimpleProjectile.SimpleProjectile_C'"));
    if (light.Object)
        b1 = light.Object;
    static ConstructorHelpers::FObjectFinder<UClass> heavy(TEXT("Class'/Game/Mods/Common/Projectiles/HeavyProjectile.HeavyProjectile_C'"));
    if (heavy.Object)
        b2 = heavy.Object;
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
void AGlider::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    float rpm1 = 60.f / 400.0f;
    float rpm2 = 60.f / 60.0f;
    
    const auto GunOffsetLeft    = FVector(150.0f, -100.0f, 0.0f);
    const auto GunOffsetRight   = FVector(150.0f, 100.0f, 0.0f);
    const auto GunOffsetTop     = FVector(150.0f, 0.0f, 100.0f);
    
    const FRotator SpawnRotation = GetControlRotation();

    const FVector SpawnLocationLeft = GetActorLocation() + SpawnRotation.RotateVector(GunOffsetLeft);
    const FVector SpawnLocationRight = GetActorLocation() + SpawnRotation.RotateVector(GunOffsetRight);
    const FVector SpawnLocationTop = GetActorLocation() + SpawnRotation.RotateVector(GunOffsetTop);

    time1 -= DeltaTime;
    time2 -= DeltaTime;

    if (FireLight)
    {
        if (time1 < 0)
        {
            if (b1)
            {
                if (LeftGun)
                GetWorld()->SpawnActor(b1, &SpawnLocationLeft, &SpawnRotation);
                else
                GetWorld()->SpawnActor(b1, &SpawnLocationRight, &SpawnRotation);
                LeftGun = !LeftGun;
            }
            time1 = rpm1;
        }
    }
    if (FireHeavy)
    {
        if (time2 < 0)
        {
            if (b2)
                GetWorld()->SpawnActor(b2, &SpawnLocationTop, &SpawnRotation);
            time2 = rpm2;
        }
    }

    FVector NewLocation = GetActorLocation();
    float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
    NewLocation.Z += DeltaHeight * 1.0f;       //Scale our height by a factor of 20
    RunningTime += DeltaTime;
    SetActorLocation(NewLocation);

    // move
    if (!CurrentVelocity.IsZero())
    {
        FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
        SetActorLocation(NewLocation);
    }
}

// Called to bind functionality to input
void AGlider::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
    
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
    
    InputComponent->BindAction("FireLight", IE_Pressed, this, &AGlider::FireLightOn);
    InputComponent->BindAction("FireLight", IE_Released, this, &AGlider::FireLightOff);
    InputComponent->BindAction("FireHeavy", IE_Pressed, this, &AGlider::FireHeavyOn);
    InputComponent->BindAction("FireHeavy", IE_Released, this, &AGlider::FireHeavyOff);
}

void AGlider::Move(float AxisValue)
{
    AxisValue *= 100;
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
    AxisValue *= 100;
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
    //case FPSWeapons:
        FirstPersonCameraComponent->SetActive(true);
        ThirdPersonCameraComponent->SetActive(false);
        VisibleComponent->SetOwnerNoSee(true);
        break;
    case TPS:
        FirstPersonCameraComponent->SetActive(false);
        ThirdPersonCameraComponent->SetActive(true);
        VisibleComponent->SetOwnerNoSee(false);
        break;
    }
}
void AGlider::FireLightOn()
{
    FireLight = true;
}

void AGlider::FireLightOff()
{
    FireLight = false;
}

void AGlider::FireHeavyOn()
{
    FireHeavy = true;
}

void AGlider::FireHeavyOff()
{
    FireHeavy = false;
}