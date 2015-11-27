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

#include "P4Glider.h"
#include "Projectile.h"

#include "GliderMovement.h"

AP4Glider::AP4Glider()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    // Set this pawn to be controlled by the lowest-numbered player
    //AutoPossessPlayer = EAutoReceiveInput::Player0;

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // Create a dummy root component we can attach things to.
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    //MovementComponent = CreateDefaultSubobject<UGliderMovement>(TEXT("CustomMovementComponent"));
    //MovementComponent->UpdatedComponent = RootComponent;
    	
    VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleComponent"));
    
    //static ConstructorHelpers::FObjectFinder<UStaticMesh> GliderMesh(TEXT("/Game/Mods/Common/Gliders/Hawk/GLD_M1_BASE"));
    //if (GliderMesh.Succeeded())
    {
        //VisibleComponent->SetStaticMesh(GliderMesh.Object);
        //VisibleComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
    }
    VisibleComponent->AttachTo(RootComponent);

    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
    FirstPersonCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    FirstPersonCameraComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    FirstPersonCameraComponent->bUsePawnControlRotation = true;
    FirstPersonCameraComponent->AttachTo(VisibleComponent);

    ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TPSCamera"));
    ThirdPersonCameraComponent->SetRelativeLocation(FVector(-1000.0f, 0.0f, 250.0f));
    ThirdPersonCameraComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    ThirdPersonCameraComponent->bUsePawnControlRotation = false;
    ThirdPersonCameraComponent->AttachTo(RootComponent);

    // set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
    
    //bUseControllerRotationPitch = false;
    //bUseControllerRotationYaw = true;
    //bUseControllerRotationRoll = true;

    VisibleComponent->SetSimulatePhysics(false);

    UpdateView();

    static ConstructorHelpers::FObjectFinder<UClass> light(TEXT("Class'/Game/Mods/Common/Projectiles/SimpleProjectile.SimpleProjectile_C'"));
    if (light.Object)
        b1 = light.Object;
    static ConstructorHelpers::FObjectFinder<UClass> heavy(TEXT("Class'/Game/Mods/Common/Projectiles/HeavyProjectile.HeavyProjectile_C'"));
    if (heavy.Object)
        b2 = heavy.Object;
}

// Called when the game starts or when spawned
void AP4Glider::BeginPlay()
{
	Super::BeginPlay();
}

/*UPawnMovementComponent* AP4Glider::GetMovementComponent() const
{
    return MovementComponent;
}*/

// Called every frame
void AP4Glider::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    auto component_loc = VisibleComponent->GetComponentLocation();
    auto ControlRotation = GetControlRotation();
    ControlRotation.Roll = 0;
    RootComponent->SetWorldRotation(ControlRotation);

    float rpm1 = 60.f / 400.0f;
    float rpm2 = 60.f / 60.0f;
    
    const auto GunOffsetLeft    = FVector(150.0f, -100.0f, 0.0f);
    const auto GunOffsetRight   = FVector(150.0f, 100.0f, 0.0f);
    const auto GunOffsetTop     = FVector(150.0f, 0.0f, 100.0f);
    
    FRotator SpawnRotation = GetControlRotation();
    SpawnRotation.Roll = 0;

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
                {
                    AProjectile *p = (AProjectile *)GetWorld()->SpawnActor(b1, &SpawnLocationLeft, &SpawnRotation);
                    //p->SetOwner(VisibleComponent);
                }
                else
                {
                    AProjectile *p = (AProjectile *)GetWorld()->SpawnActor(b1, &SpawnLocationRight, &SpawnRotation);
                    //p->SetOwner(VisibleComponent->StaticMesh);
                }
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

    // move
    if (!CurrentVelocity.IsZero())
    {
        NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
    }

    SetActorLocation(NewLocation);
}

// Called to bind functionality to input
void AP4Glider::SetupPlayerInputComponent(class UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);
    
    InInputComponent->BindAction("View", IE_Pressed, this, &AP4Glider::ChangeView);

    InInputComponent->BindAxis("Move", this, &AP4Glider::Move);
    InInputComponent->BindAxis("Strafe", this, &AP4Glider::Strafe);
    
    //InInputComponent->BindAxis("Turn", this, &AP4Glider::Turn);
	//InInputComponent->BindAxis("LookUp", this, &AP4Glider::LookUp);
    InInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    InInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

    InInputComponent->BindAction("Jump", IE_Pressed, this, &AP4Glider::Jump);
    //InInputComponent->BindAction("Jump", IE_Released, this, &AP4Glider::Jump);
    
    InInputComponent->BindAction("Boost", IE_Pressed, this, &AP4Glider::BoostOn);
    InInputComponent->BindAction("Boost", IE_Released, this, &AP4Glider::BoostOff);
    
    InInputComponent->BindAction("FireLight", IE_Pressed, this, &AP4Glider::FireLightOn);
    InInputComponent->BindAction("FireLight", IE_Released, this, &AP4Glider::FireLightOff);
    InInputComponent->BindAction("FireHeavy", IE_Pressed, this, &AP4Glider::FireHeavyOn);
    InInputComponent->BindAction("FireHeavy", IE_Released, this, &AP4Glider::FireHeavyOff);
}

void AP4Glider::Move(float AxisValue)
{
    AxisValue *= 100;
    if (Controller != NULL && AxisValue != 0.0f)
    {
        FRotator Rotation = Controller->GetControlRotation();
        Rotation.Pitch = 0;
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
        AddMovementInput(Direction, AxisValue);

        auto pc = Cast<UPrimitiveComponent>(GetRootComponent());
        if (pc)
            pc->AddForce(Direction);
    }
    //if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
    {
        //MovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
    }
}

void AP4Glider::Strafe(float AxisValue)
{
    AxisValue *= 100;
    if (Controller != NULL && AxisValue != 0.0f)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
        AddMovementInput(Direction, AxisValue);
    }
    //if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
    {
        //MovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
    }
}

void AP4Glider::Jump()
{
}

void AP4Glider::BoostOn()
{
    boost = true;
}

void AP4Glider::BoostOff()
{
    boost = false;
}
    
void AP4Glider::Turn(float AxisValue)
{
    FRotator NewRotation = GetActorRotation();
    NewRotation.Yaw += AxisValue;
    SetActorRotation(NewRotation);
}

void AP4Glider::LookUp(float AxisValue)
{
    FRotator NewRotation = GetActorRotation();
    NewRotation.Pitch += AxisValue;
    SetActorRotation(NewRotation);
}

void AP4Glider::ChangeView()
{
    GliderView = (GliderView + 1) % EGliderView::Max;
    UpdateView();
}

void AP4Glider::UpdateView()
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

void AP4Glider::FireLightOn()
{
    FireLight = true;
}

void AP4Glider::FireLightOff()
{
    FireLight = false;
}

void AP4Glider::FireHeavyOn()
{
    FireHeavy = true;
}

void AP4Glider::FireHeavyOff()
{
    FireHeavy = false;
}
