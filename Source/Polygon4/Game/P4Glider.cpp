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

FPowerUpProperties::FPowerUpProperties()
{
    HoverInAir = true;
    HoverAltitude = 250.0f;
    GravityScale = 0.2f;
    LiftSpring = 350000.0f;
    LiftDamp = -128.0f;
    SlowFall = false;
    FallSpeed = 100.0f;
    SelfRight = true;
    AlphaTrack = 500000.0f;
    AlphaDamp = -128.0f;
}

AP4Glider::AP4Glider()
{
	PrimaryActorTick.bCanEverTick = true;
    
    //AutoPossessPlayer = EAutoReceiveInput::Player0;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    //RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    //MovementComponent = CreateDefaultSubobject<UGliderMovement>(TEXT("CustomMovementComponent"));
    //MovementComponent->UpdatedComponent = RootComponent;

    VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleComponent"));
    RootComponent = VisibleComponent;
    Mesh = VisibleComponent;

    JumpSound = CreateDefaultSubobject<UAudioComponent>(TEXT("SoundWave'/Game/Mods/Common/Sounds/Glider/jump.jump'"));
    //JumpSound = CreateDefaultSubobject<UAudioComponent>(TEXT("SoundCue'/Game/Mods/Common/Sounds/Glider/jump_Cue.jump_Cue'"));
    //if (!JumpSound)
    //    JumpSound = CreateDefaultSubobject<UAudioComponent>(TEXT("/Game/Mods/Common/Sounds/Glider/jump"));
    if (JumpSound)
    {
        JumpSound->AttachParent = RootComponent;
        JumpSound->bAutoActivate = false;
    }

        
    //static ConstructorHelpers::FObjectFinder<UStaticMesh> GliderMesh(TEXT("/Game/Mods/Common/Gliders/Hawk/GLD_M1_BASE"));
    //if (GliderMesh.Succeeded())
    {
        //VisibleComponent->SetStaticMesh(GliderMesh.Object);
        //VisibleComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
    }
    //VisibleComponent->SnapTo(RootComponent);

    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
    //FirstPersonCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    //FirstPersonCameraComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    FirstPersonCameraComponent->bUsePawnControlRotation = true;
    FirstPersonCameraComponent->AttachTo(VisibleComponent);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPSCameraSpringArm"));
    //SpringArm->SetRelativeLocation(FVector(-1000.0f, 0.0f, 250.0f));
    //SpringArm->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    SpringArm->bUsePawnControlRotation = false;
    SpringArm->AttachTo(RootComponent);
    SpringArm->RelativeRotation = FRotator(-10.f, 0.f, 0.f);
    SpringArm->TargetArmLength = 500.0f;
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 5.0f;

    ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TPSCamera"));
    //ThirdPersonCameraComponent->SetRelativeLocation(FVector(-1000.0f, 0.0f, 250.0f));
    //ThirdPersonCameraComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    //ThirdPersonCameraComponent->bUsePawnControlRotation = false;
    //ThirdPersonCameraComponent->AttachTo(RootComponent);
    ThirdPersonCameraComponent->AttachTo(SpringArm, USpringArmComponent::SocketName);

    // set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
    
    //bUseControllerRotationPitch = false;
    //bUseControllerRotationYaw = true;
    //bUseControllerRotationRoll = false;

    VisibleComponent->SetSimulatePhysics(true);

    UpdateView();

    static ConstructorHelpers::FObjectFinder<UClass> light(TEXT("Class'/Game/Mods/Common/Projectiles/SimpleProjectile.SimpleProjectile_C'"));
    if (light.Object)
        b1 = light.Object;
    static ConstructorHelpers::FObjectFinder<UClass> heavy(TEXT("Class'/Game/Mods/Common/Projectiles/HeavyProjectile.HeavyProjectile_C'"));
    if (heavy.Object)
        b2 = heavy.Object;

    JumpTimeout = ArmedTimedValue(1.5);
}

void AP4Glider::BeginPlay()
{
	Super::BeginPlay();
}

//UPawnMovementComponent* AP4Glider::GetMovementComponent() const
//{
//    return MovementComponent;
//}

void AP4Glider::Tick(float DeltaTime)
{
    // parent
    Super::Tick(DeltaTime);

    // timers
    JumpTimeout += DeltaTime;

    // trace
    ZTraceResults = HoverTrace(50000);

    //

    //
    float rpm1 = 60.f / 400.0f;
    float rpm2 = 60.f / 60.0f;
    
    const auto GunOffsetLeft    = FVector(150.0f, -100.0f, 0.0f);
    const auto GunOffsetRight   = FVector(150.0f, 100.0f, 0.0f);
    const auto GunOffsetTop     = FVector(150.0f, 0.0f, 100.0f);
    
    FRotator SpawnRotation = GetActorRotation();
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
    

    float Altitude = FVector(GetActorLocation() - ZTraceResults.ImpactPoint).Size();
    float g = 980;
    if (Altitude > PowerUpProperties.HoverAltitude + 1000)
    {
        g = -g;
    }
    else if (Altitude > PowerUpProperties.HoverAltitude + 10)
    {
        g = (PowerUpProperties.HoverAltitude - Altitude) / 2;
        //UE_LOG(LogTemp, Warning, TEXT("Altitude: %f"), Altitude);
    }
    else if (Altitude < PowerUpProperties.HoverAltitude)
    {
        g -= (Altitude - PowerUpProperties.HoverAltitude) * 2;
    }

    VisibleComponent->SetLinearDamping(1.0f);
    VisibleComponent->SetAngularDamping(1.0f);
    VisibleComponent->AddForce(FVector::UpVector * VisibleComponent->GetBodyInstance()->GetBodyMass() * g);




    // Get the Compression Ratio of our invisible 'Spring'. This should always be between 1 and zero, and clamped to zero. */
    /*float CompressionRatio;
    float RatioA = Altitude / (PowerUpProperties.HoverAltitude * -1.0f);

    if (RatioA < -1.0f)
    {
        CompressionRatio = 0.0f;
    }
    else
    {
        CompressionRatio = FMath::GetMappedRangeValueClamped(FVector2D(0.0, -1.0), FVector2D(1.0f, 0.0f), RatioA);
    }

    // Now add the spring force to the crate.
    FVector SpringForce = FVector(0.0f, 0.0f, (CompressionRatio * PowerUpProperties.LiftSpring) + (PowerUpProperties.LiftDamp * Mesh->GetPhysicsLinearVelocity().Z));
    Mesh->AddForce(SpringForce);

    /* Now we need to apply angular torque to the create to try and make it follow the normal of the terrain/object it's 'landed' on. 
    /* First, get the Normal of the ground and Normalize it, with a slight bias in the upwards direction. 
    FVector TraceNormal = FVector(ZTraceResults.ImpactNormal.X, ZTraceResults.ImpactNormal.Y, ZTraceResults.ImpactNormal.Z * 2.0f);
    TraceNormal.Normalize();

    /* Use our value to set the 'Damping' Force from our 'AlphaDamp' value. We also do this in 'Z' to prevent cone-spinning. 
    FVector Alpha;
    Alpha.X = PowerUpProperties.AlphaDamp * Mesh->GetPhysicsAngularVelocity().X;
    Alpha.Y = PowerUpProperties.AlphaDamp * Mesh->GetPhysicsAngularVelocity().Y;
    Alpha.Z = PowerUpProperties.AlphaDamp * Mesh->GetPhysicsAngularVelocity().Z;

    /* Scale the speed of the self-righting based on our current compression ratio (more torque at the top of the springs length). 
    float ScaledTrack = PowerUpProperties.AlphaTrack * (CompressionRatio * -1.0f);

    /* Set the new Alpha Values, attempt to align with the Forward and Right vectors, which should then straighten our Up vector. 
    //Alpha.X += ScaledTrack * FVector::DotProduct(TraceNormal, GetActorForwardVector());
    //Alpha.Y += ScaledTrack * FVector::DotProduct(TraceNormal, GetActorRightVector());
    Alpha.Z += ScaledTrack * FVector::DotProduct(TraceNormal, GetActorUpVector());

    // Apply the torque and scale it by the crate mass so our values stay slightly more sensible. (Torque is really weak by default). 
    Mesh->AddTorque(Alpha * Mesh->GetMass());*/




    auto component_loc = VisibleComponent->GetComponentLocation();
    auto ControlRotation = GetControlRotation();
    ControlRotation.Roll = 0;
    RootComponent->SetWorldRotation(ControlRotation);
    //Mesh->AddTorque(  * Mesh->GetMass());




    

    //Hover();
    //VisibleComponent->SetWorldRotation(GetActorRotation());
    //VisibleComponent->SetWorldLocation(GetActorLocation());

    // at the end
}

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
    if (Controller != NULL && AxisValue != 0.0f)
    {
        AxisValue *= boost ? 2000 : 1000;
        //FRotator Rotation = Controller->GetControlRotation();
        //const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
        //AddMovementInput(Direction, AxisValue);
        
        //AddMovementInput(GetActorForwardVector() * AxisValue);
        Mesh->AddForce(GetActorForwardVector() * VisibleComponent->GetBodyInstance()->GetBodyMass() * AxisValue);

        //auto pc = Cast<UPrimitiveComponent>(GetRootComponent());
        //if (pc)
        //    pc->AddForce(Direction);
    }
    //if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
    {
        //MovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
    }
}

void AP4Glider::Strafe(float AxisValue)
{
    if (Controller != NULL && AxisValue != 0.0f)
    {
        AxisValue *= 1000;
        //const FRotator Rotation = Controller->GetControlRotation();
        //const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
        //AddMovementInput(Direction, AxisValue);

        //AddMovementInput(GetActorRightVector() * AxisValue);
        Mesh->AddForce(GetActorRightVector() * VisibleComponent->GetBodyInstance()->GetBodyMass() * AxisValue);
    }
    //if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
    {
        //MovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
    }
}

void AP4Glider::Jump()
{
    if (JumpTimeout)
    {
        VisibleComponent->AddForce(GetActorUpVector() * VisibleComponent->GetMass() * 30, NAME_None, true);
        JumpSound->Activate(true);
        JumpSound->Play();
    }
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

void AP4Glider::Hover()
{
    FHitResult TraceResults = HoverTrace(PowerUpProperties.HoverAltitude);

    auto Mesh = VisibleComponent;

    /* If the Trace Hit Something, then we need to start applying the hovering physics. Otherwise, we let the Physics Engine Handle Everything. */
    if (TraceResults.bBlockingHit)
    {
        // Firstly, slow down the lateral movement so we don't go flying off into the wilderness.
        Mesh->SetLinearDamping(1.0f);

        /* Get the Compression Ratio of our invisible 'Spring'. This should always be between 1 and zero, and clamped to zero. */
        float CompressionRatio;

        /* Get Compression Ratio / Height Ratio */
        float Altitude = FVector(GetActorLocation() - TraceResults.ImpactPoint).Size();
        float RatioA = Altitude / (PowerUpProperties.HoverAltitude * -1.0f);

        if (RatioA < -1.0f)
        {
            CompressionRatio = 0.0f;
        }
        else
        {
            CompressionRatio = FMath::GetMappedRangeValueClamped(FVector2D(0.0, -1.0), FVector2D(1.0f, 0.0f), RatioA);
        }

        /* Now add the spring force to the crate. */
        FVector SpringForce = FVector(0.0f, 0.0f, (CompressionRatio * PowerUpProperties.LiftSpring) + (PowerUpProperties.LiftDamp * Mesh->GetPhysicsLinearVelocity().Z));
        Mesh->AddForce(SpringForce);

        /* Now we need to apply angular torque to the create to try and make it follow the normal of the terrain/object it's 'landed' on. */
        /* First, get the Normal of the ground and Normalize it, with a slight bias in the upwards direction. */
        FVector TraceNormal = FVector(TraceResults.ImpactNormal.X, TraceResults.ImpactNormal.Y, TraceResults.ImpactNormal.Z * 2.0f);
        TraceNormal.Normalize();

        /* Use our value to set the 'Damping' Force from our 'AlphaDamp' value. We also do this in 'Z' to prevent cone-spinning. */
        FVector Alpha;
        Alpha.X = PowerUpProperties.AlphaDamp * Mesh->GetPhysicsAngularVelocity().X;
        Alpha.Y = PowerUpProperties.AlphaDamp * Mesh->GetPhysicsAngularVelocity().Y;
        Alpha.Z = PowerUpProperties.AlphaDamp * Mesh->GetPhysicsAngularVelocity().Z;

        /* Scale the speed of the self-righting based on our current compression ratio (more torque at the top of the springs length). */
        float ScaledTrack = PowerUpProperties.AlphaTrack * (CompressionRatio * -1.0f);

        /* Set the new Alpha Values, attempt to align with the Forward and Right vectors, which should then straighten our Up vector. */
        Alpha.X += ScaledTrack * FVector::DotProduct(TraceNormal, GetActorForwardVector());
        Alpha.Y += ScaledTrack * FVector::DotProduct(TraceNormal, GetActorRightVector());

        /* Apply the torque and scale it by the crate mass so our values stay slightly more sensible. (Torque is really weak by default). */
        Mesh->AddTorque(Alpha * Mesh->GetMass());
    }
    else
    {
        // If we don't hit anything, let the crate fall with minimal resistance.
        Mesh->SetLinearDamping(0.01f);
    }
}

FHitResult AP4Glider::HoverTrace(float Altitude) const
{
    static const FName HoverTraceTag("HoverTrace");
    //GetWorld()->DebugDrawTraceTag = HoverTraceTag;

    FCollisionQueryParams TraceParams(HoverTraceTag, true, this);
    TraceParams.bTraceAsyncScene = true;
    //TraceParams.TraceTag = HoverTraceTag;

    auto begin = GetActorLocation();
    auto end = begin;
    end.Z -= Altitude;
    
    FHitResult Hit(ForceInit);
    if (GetWorld()->LineTraceSingleByChannel(
        Hit,
        begin,
        end,
        ECollisionChannel::ECC_Visibility,
        TraceParams))
    {
        //UE_LOG(LogTemp, Warning, TEXT("LineTraceSingleByChannel succeed"));
        return Hit;
    }

    if (GetWorld()->LineTraceSingleByObjectType(
        Hit,
        begin,
        end,
        ECollisionChannel::ECC_Visibility, //
        TraceParams))
    {
        UE_LOG(LogTemp, Warning, TEXT("LineTraceSingleByObjectType succeed"));
        return Hit;
    }

    if (GetWorld()->LineTraceSingleByChannel(
        Hit,
        begin,
        end,
        ECollisionChannel::ECC_PhysicsBody,
        TraceParams))
    {
        UE_LOG(LogTemp, Warning, TEXT("LineTraceSingleByChannel ECC_PhysicsBody succeed"));
        return Hit;
    }

    if (GetWorld()->LineTraceSingleByObjectType(
        Hit,
        begin,
        end,
        ECollisionChannel::ECC_PhysicsBody, //
        TraceParams))
    {
        UE_LOG(LogTemp, Warning, TEXT("LineTraceSingleByObjectType ECC_PhysicsBody succeed"));
        return Hit;
    }

    if (GetWorld()->LineTraceSingleByChannel(
        Hit,
        begin,
        end,
        ECollisionChannel::ECC_WorldStatic,
        TraceParams))
    {
        UE_LOG(LogTemp, Warning, TEXT("LineTraceSingleByChannel ECC_WorldStatic succeed"));
        return Hit;
    }

    if (GetWorld()->LineTraceSingleByObjectType(
        Hit,
        begin,
        end,
        ECollisionChannel::ECC_WorldStatic, //
        TraceParams))
    {
        UE_LOG(LogTemp, Warning, TEXT("LineTraceSingleByObjectType ECC_WorldStatic succeed"));
        return Hit;
    }

    if (GetWorld()->LineTraceSingleByChannel(
        Hit,
        begin,
        end,
        ECollisionChannel::ECC_WorldDynamic,
        TraceParams))
    {
        UE_LOG(LogTemp, Warning, TEXT("LineTraceSingleByChannel ECC_WorldDynamic succeed"));
        return Hit;
    }

    if (GetWorld()->LineTraceSingleByObjectType(
        Hit,
        begin,
        end,
        ECollisionChannel::ECC_WorldDynamic, //
        TraceParams))
    {
        UE_LOG(LogTemp, Warning, TEXT("LineTraceSingleByObjectType ECC_WorldDynamic succeed"));
        return Hit;
    }

    return Hit;
}
