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
#include "GliderHUD.h"

const float k_mouse_x = 0.6;
const float k_mouse_y = 0.6;

FPowerUpProperties::FPowerUpProperties()
{
    HoverInAir = true;
    HoverAltitude = 300.0f;
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

    //CenterPoint = CreateDefaultSubobject<USceneComponent>(TEXT("CenterPoint"));

    VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleComponent"));
    RootComponent = VisibleComponent;
    Mesh = VisibleComponent;

    JumpSound = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
    //JumpSound = CreateDefaultSubobject<UAudioComponent>(TEXT("SoundCue'/Game/Mods/Common/Sounds/Glider/jump_Cue.jump_Cue'"));
    //if (!JumpSound)
    //    JumpSound = CreateDefaultSubobject<UAudioComponent>(TEXT("/Game/Mods/Common/Sounds/Glider/jump"));
    if (JumpSound)
    {
        JumpSound->AttachParent = RootComponent;
        JumpSound->bAutoActivate = false;
    }

    JumpSound2 = CreateDefaultSubobject<USoundWave>(TEXT("SoundWave'/Game/Mods/Common/Sounds/Glider/jump.jump'"));

        
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
    //FirstPersonCameraComponent->bUsePawnControlRotation = true;
    FirstPersonCameraComponent->AttachTo(VisibleComponent);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPSCameraSpringArm"));
    //SpringArm->SetRelativeLocation(FVector(-1000.0f, 0.0f, 250.0f));
    //SpringArm->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    SpringArm->bUsePawnControlRotation = false;
    SpringArm->AttachTo(VisibleComponent);
    SpringArm->RelativeRotation = FRotator(-20.f, 0.f, 0.f);
    SpringArm->TargetArmLength = 500.0f;
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 5.0f;

    ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TPSCamera"));
    //ThirdPersonCameraComponent->SetRelativeLocation(FVector(-1000.0f, 0.0f, 250.0f));
    //ThirdPersonCameraComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    ThirdPersonCameraComponent->bUsePawnControlRotation = false;
    //ThirdPersonCameraComponent->AttachTo(RootComponent);
    ThirdPersonCameraComponent->AttachTo(SpringArm, USpringArmComponent::SocketName);

    // set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
    
    //bUseControllerRotationPitch = false;
    //bUseControllerRotationYaw = true;
    //bUseControllerRotationRoll = false;

    VisibleComponent->SetSimulatePhysics(true);
    VisibleComponent->SetPhysicsMaxAngularVelocity(30);

    UpdateView();

    static ConstructorHelpers::FObjectFinder<UClass> light(TEXT("Class'/Game/Mods/Common/Projectiles/SimpleProjectile.SimpleProjectile_C'"));
    if (light.Object)
        b1 = light.Object;
    static ConstructorHelpers::FObjectFinder<UClass> heavy(TEXT("Class'/Game/Mods/Common/Projectiles/HeavyProjectile.HeavyProjectile_C'"));
    if (heavy.Object)
        b2 = heavy.Object;

    JumpTimeout = ArmedTimedValue(1.5);
    SlopeAngle = FloatDampingValue(30);
}

void AP4Glider::BeginPlay()
{
    JumpSound->Activate(true);

    SpringArm->bInheritRoll = false;
    SpringArm->bInheritPitch = true;
    SpringArm->bInheritYaw = true;

    Super::BeginPlay();

    //JumpSound->Activate(true);
}

//UPawnMovementComponent* AP4Glider::GetMovementComponent() const
//{
//    return MovementComponent;
//}

void AP4Glider::Tick(float DeltaSeconds)
{
    // parent
    Super::Tick(DeltaSeconds);

    APlayerController* PlayerController = Cast<APlayerController>(GetController());

    // mouse pos
    if (PlayerController)
    {
        auto Viewport = GetWorld()->GetGameViewport();
        FIntPoint ViewSize = Viewport->Viewport->GetSizeXY();
        FVector2D Position;
        bool Focused1 = Viewport->IsFocused(Viewport->Viewport);
        bool Focused2 = Viewport->Viewport->HasFocus();
        if ((Focused1 || Focused2) && PlayerController->GetMousePosition(Position.X, Position.Y))
        {
            enum class MouseChange
            {
                None,
                Start,
                End,
            };
            auto NormPos = [](float &x, int len, float k)
            {
                int window = len * k;
                int pos = round(x);
                int start = (len - window) / 2.0f;
                int end = start + window;
                MouseChange changed = MouseChange::None;
                if (pos > end)
                {
                    pos = end;
                    changed = MouseChange::End;
                }
                else if (pos < start)
                {
                    pos = start;
                    changed = MouseChange::Start;
                }
                x = pos;
                return changed;
            };

            static auto oldpos2 = Position;
            if (oldpos2.X != Position.X)
            {
                PlayerController->GetMousePosition(Position.X, Position.Y);
            }
            oldpos2 = Position;

            //UE_LOG(LogTemp, Warning, TEXT("mouse before: x = %f, y = %f"), Position.X, Position.Y);

            auto oldpos = Position;
            auto changed_x = NormPos(Position.X, ViewSize.X, k_mouse_x);
            auto changed_y = NormPos(Position.Y, ViewSize.Y, k_mouse_y);
            bool changed = changed_x != MouseChange::None || changed_y != MouseChange::None;

            if (changed)
            {
                //UE_LOG(LogTemp, Warning, TEXT("mouse before: x = %f, y = %f"), oldpos.X, oldpos.Y);
                //UE_LOG(LogTemp, Warning, TEXT("mouse after : x = %f, y = %f"), Position.X, Position.Y);

                //changed_x = NormPos(oldpos.X, ViewSize.X, k_mouse_x);
                //changed_y = NormPos(oldpos.Y, ViewSize.Y, k_mouse_y);

                if (changed_y == MouseChange::End)
                    Position.X++;

                //UE_LOG(LogTemp, Warning, TEXT("mouse after2: x = %f, y = %f"), oldpos.X, oldpos.Y);

                Viewport->Viewport->SetMouse(Position.X, Position.Y);

                //PlayerController->GetMousePosition(Position.X, Position.Y);

                //UE_LOG(LogTemp, Warning, TEXT("mouse after3: x = %f, y = %f"), Position.X, Position.Y);
                //UE_LOG(LogTemp, Warning, TEXT("-------------------------------------"));
            }

            auto HUD = Cast<AGliderHUD>(PlayerController->GetHUD());
            HUD->SetMousePosition(Position);

            //
            FIntPoint Center = ViewSize / 2;
            auto kx = (Position.X - Center.X) / (float)Center.X;
            auto ky = (Position.Y - Center.Y) / (float)Center.Y;
            Position.X -= copysignf(3, kx);
            Position.Y -= copysignf(2, ky);

            //Viewport->Viewport->SetMouse(Position.X, Position.Y);
            //UE_LOG(LogTemp, Warning, TEXT("mouse after3: x = %f, y = %f"), Position.X, Position.Y);
        }
    }

    // timers
    JumpTimeout += DeltaSeconds;

    //

    //
    float rpm1 = 60.f / 400.0f;
    float rpm2 = 60.f / 60.0f;
    
    const auto GunOffsetLeft    = FVector(150.0f, -100.0f, 0.0f);
    const auto GunOffsetRight   = FVector(150.0f, 100.0f, 0.0f);
    const auto GunOffsetTop     = FVector(150.0f, 0.0f, 100.0f);
    
    FRotator SpawnRotation = GetActorRotation();
    if (PlayerController)
    {
        FVector loc;
        FVector rot;
        PlayerController->DeprojectMousePositionToWorld(loc, rot);
        SpawnRotation = rot.Rotation();
    }
    SpawnRotation.Roll = 0;

    const FVector SpawnLocationLeft = GetActorLocation() + SpawnRotation.RotateVector(GunOffsetLeft);
    const FVector SpawnLocationRight = GetActorLocation() + SpawnRotation.RotateVector(GunOffsetRight);
    const FVector SpawnLocationTop = GetActorLocation() + SpawnRotation.RotateVector(GunOffsetTop);

    time1 -= DeltaSeconds;
    time2 -= DeltaSeconds;

    if (FireLight)
    {
        if (time1 < 0)
        {
            if (b1)
            {
                if (LeftGun)
                {
                    AProjectile *p = (AProjectile *)GetWorld()->SpawnActor(b1, &SpawnLocationLeft, &SpawnRotation);
                    p->SetOwner(VisibleComponent);
                }
                else
                {
                    AProjectile *p = (AProjectile *)GetWorld()->SpawnActor(b1, &SpawnLocationRight, &SpawnRotation);
                    p->SetOwner(VisibleComponent);
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

    // trace
    ZTraceResults = HoverTrace(50000);
    if (!ZTraceResults.bBlockingHit)
    {
        //UE_LOG(LogTemp, Error, TEXT("Tick Error: No forces will be applied. Line: %d"), __LINE__);
        //UE_LOG(LogTemp, Error, TEXT("Actor: %d"), GetActorLabel().GetCharArray().GetData());
        return;
    }

    auto CalcAngle = [](const FVector &Vector1, const FVector &Vector2 = FVector::UpVector)
    {
        float dp = FVector::DotProduct(Vector1, Vector2);
        dp /= Vector1.Size() * Vector2.Size();
        float acos_ = acosf(dp);
        float grad = (acos_ * (180 / 3.1415926));
        if (isnan(grad))
            grad = 90.0;
        return grad;
    };

    SlopeAngle += CalcAngle(ZTraceResults.ImpactNormal);

    //if (PlayerController)
    {
        // recalc angle
        //CurrentAngle = CalcAngle();
    }

    const float angle_max = 45.0f; // make angle - slow value (like average on last 5 values) - damping
    bool on_slope = SlopeAngle > angle_max;
    float f = SlopeAngle;
    //if (PlayerController)
    //    UE_LOG(LogTemp, Warning, TEXT("angle = %f"), f);
    if (on_slope)
    {
        //if (PlayerController)
        //    UE_LOG(LogTemp, Warning, TEXT("angle before = %f"), angle);
        auto old = ZTraceResults.ImpactNormal;
        ZTraceResults = HoverTrace(50000, ZTraceResults.ImpactNormal);
        if (!ZTraceResults.bBlockingHit)
        {
            //UE_LOG(LogTemp, Error, TEXT("Tick Error: No forces will be applied. Line: %d"), __LINE__);
            return;
        }
        //CurrentAngle = CalcAngle(ZTraceResults.ImpactNormal);
        //if (PlayerController)
        //    UE_LOG(LogTemp, Warning, TEXT("angle after = %f"), angle);
    }


    float Altitude = ZTraceResults.Distance;
    float g = 980;
    float force = g;
    if (!on_slope)
    {
        if (Altitude > PowerUpProperties.HoverAltitude + 1000) // force down
        {
            force = -g;
        }
        else if (Altitude > PowerUpProperties.HoverAltitude + 50) // small down
        {
            force = (PowerUpProperties.HoverAltitude - Altitude) / 2;
        }
        else if (Altitude < PowerUpProperties.HoverAltitude) // force up
        {
            //force = g - (Altitude - PowerUpProperties.HoverAltitude) * 3;
            force = g * (1 + 3 * (PowerUpProperties.HoverAltitude - Altitude) / PowerUpProperties.HoverAltitude);
        }
        VisibleComponent->SetLinearDamping(0.5f);
        VisibleComponent->SetAngularDamping(0.75f);
    }
    else
    {
        float slope_altitude = PowerUpProperties.HoverAltitude;// / 2;
        if (Altitude > slope_altitude) // small down
        {
            force = (slope_altitude - Altitude) / 2;
        }
        else if (Altitude < slope_altitude) // force up
        {
            force = g - (Altitude - slope_altitude) * 2;
        }
        VisibleComponent->SetLinearDamping(1.0f);
        VisibleComponent->SetAngularDamping(1.0f);
    }
    


    //if (!isnan(angle) && PlayerController)
    {
        //UE_LOG(LogTemp, Warning, TEXT("angle = %f"), angle);
    }
    // force can be applied
    FVector ForceVector = FVector::UpVector;
    if (on_slope)
    {
        ForceVector = ZTraceResults.ImpactNormal;
    }
    VisibleComponent->AddForce(ForceVector * VisibleComponent->GetBodyInstance()->GetBodyMass() * force);




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



    // Torque
    {
        FVector Torque = ZTraceResults.ImpactNormal;
        if (PlayerController)
        {
            if (GliderView != EGliderView::TPS)
            {
                FVector2D Position;
                if (PlayerController->GetMousePosition(Position.X, Position.Y))
                {
                    auto Viewport = GetWorld()->GetGameViewport();
                    FIntPoint ViewSize = Viewport->Viewport->GetSizeXY();
                    FIntPoint Center = ViewSize / 2;
                    Position.X = (Position.X - Center.X) / (float)Center.X;
                    Position.Y = (Position.Y - Center.Y) / (float)Center.Y;

                    // left/right
                    Mesh->AddTorque(Position.X * GetActorUpVector() * 10000 * Mesh->GetMass());
                    // top/bottom
                    Mesh->AddTorque(Position.Y * GetActorRightVector() * 10000 * Mesh->GetMass());
                    // do not roll
                    float RollAngle = CalcAngle(ZTraceResults.ImpactNormal, GetActorRightVector());
                    auto diff = RollAngle - 90.0f;
                    int sign = 1;
                    if (diff < 0)
                        sign = -1;
                    diff = fabs(diff);
                    //UE_LOG(LogTemp, Warning, TEXT("RollAngle = %f"), RollAngle);
                    if (diff > 20)
                        Mesh->AddTorque(sign * GetActorForwardVector() * 50000 * Mesh->GetMass());
                    else if (diff > 15)
                        Mesh->AddTorque(sign * GetActorForwardVector() * 10000 * Mesh->GetMass());
                    else if (diff > 10)
                        Mesh->AddTorque(sign * GetActorForwardVector() * 7500 * Mesh->GetMass());
                    else if (diff > 5)
                        Mesh->AddTorque(sign * GetActorForwardVector() * 2500 * Mesh->GetMass());

                    float CurrentAngle = CalcAngle(GetActorUpVector());

                    //UE_LOG(LogTemp, Warning, TEXT("CurrentAngle = %f"), CurrentAngle);
                    //if (CurrentAngle > 60)
                    //    Mesh->AddTorque(ZTraceResults.ImpactNormal ^ -GetActorUpVector() * 100000 * Mesh->GetMass());

                }
            }
            else
            {
                UCameraComponent *Camera = ThirdPersonCameraComponent;

                FVector loc;
                FVector r;
                PlayerController->DeprojectMousePositionToWorld(loc, r);
                FVector2D Position;
                PlayerController->GetMousePosition(Position.X, Position.Y);

                //UE_LOG(LogTemp, Warning, TEXT("loc: x = %f, y = %f, z = %f"), loc.X, loc.Y, loc.Z);

                FVector Direction = loc - GetActorLocation();
                //UE_LOG(LogTemp, Warning, TEXT("Direction: x = %f, y = %f, z = %f"), Direction.X, Direction.Y, Direction.Z);

                //Torque = GetActorQuat().RotateVector(Direction);
                //UE_LOG(LogTemp, Warning, TEXT("Torque: x = %f, y = %f, z = %f"), Torque.X, Torque.Y, Torque.Z);

                //Torque.Normalize();
                
                auto arot = GetActorRotation();
                auto rot = Direction.Rotation();

                /*auto diff = rot - arot;
                UE_LOG(LogTemp, Warning, TEXT("loc: %s"), loc.ToString().GetCharArray().GetData());
                UE_LOG(LogTemp, Warning, TEXT("r: %s"), r.ToString().GetCharArray().GetData());
                UE_LOG(LogTemp, Warning, TEXT("arot: %s"), arot.ToString().GetCharArray().GetData());
                UE_LOG(LogTemp, Warning, TEXT("rot: %s"), rot.ToString().GetCharArray().GetData());
                UE_LOG(LogTemp, Warning, TEXT("rot diff: %s"), diff.ToString().GetCharArray().GetData());*/


                // left/right
                //Mesh->AddTorque((diff.Pitch < 0 ? -1 : 1) * GetActorUpVector() * 30000 * Mesh->GetMass());

                //Torque = Torque ^ -GetActorUpVector();
                //Torque *= 20000;
                //Mesh->AddTorque(Torque * Mesh->GetMass());

                auto Viewport = GetWorld()->GetGameViewport();
                FIntPoint ViewSize = Viewport->Viewport->GetSizeXY();
                FIntPoint Center = ViewSize / 2;
                Position.X = (Position.X - Center.X) / (float)Center.X;
                Position.Y = (Position.Y - Center.Y) / (float)Center.Y;

                // left/right
                Mesh->AddTorque(Position.X * GetActorUpVector() * 10000 * Mesh->GetMass());
                // top/bottom
                Mesh->AddTorque(Position.Y * GetActorRightVector() * 10000 * Mesh->GetMass());
                // do not roll
                float RollAngle = CalcAngle(ZTraceResults.ImpactNormal, GetActorRightVector());
                auto diff = RollAngle - 90.0f;
                int sign = 1;
                if (diff < 0)
                    sign = -1;
                diff = fabs(diff);
                //UE_LOG(LogTemp, Warning, TEXT("RollAngle = %f"), RollAngle);
                if (diff > 20)
                    Mesh->AddTorque(sign * GetActorForwardVector() * 50000 * Mesh->GetMass());
                else if (diff > 15)
                    Mesh->AddTorque(sign * GetActorForwardVector() * 10000 * Mesh->GetMass());
                else if (diff > 10)
                    Mesh->AddTorque(sign * GetActorForwardVector() * 7500 * Mesh->GetMass());
                else if (diff > 5)
                    Mesh->AddTorque(sign * GetActorForwardVector() * 2500 * Mesh->GetMass());
            }

            //auto component_loc = VisibleComponent->GetComponentLocation();
            //auto ControlRotation = GetControlRotation();
            //ControlRotation.Roll = 0;
            //RootComponent->SetWorldRotation(ControlRotation);
        }
        else
        {
            Torque = Torque ^ -GetActorUpVector();
            Torque *= 10000;
            Mesh->AddTorque(Torque * Mesh->GetMass());
        }

        VisibleComponent->SetAngularDamping(1.0f);
    }




    

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

    InInputComponent->BindAction("HideUI", IE_Pressed, this, &AP4Glider::HideUI);
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
        VisibleComponent->AddForce(GetActorUpVector() * VisibleComponent->GetMass() * 10, NAME_None, true);
        //VisibleComponent->AddImpulse(GetActorUpVector() * 1);
        //static bool b = false;
        //if (b)
        //    JumpSound->Play();
        //else
            //UGameplayStatics::PlaySound2D(GetWorld(), JumpSound2);
        //JumpSound->Sound = JumpSound2;
        //JumpSound->Play();
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

FHitResult AP4Glider::HoverTrace(float Altitude, FVector Vector) const
{
    static const FName HoverTraceTag("HoverTrace");
    //GetWorld()->DebugDrawTraceTag = HoverTraceTag;

    FCollisionQueryParams TraceParams(HoverTraceTag, true, this);
    TraceParams.bTraceAsyncScene = true;
    //TraceParams.TraceTag = HoverTraceTag;

    auto begin = GetActorLocation();
    auto end = begin - Vector.Normalize() * Altitude;
    //end.Z -= Altitude;
    
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

void AP4Glider::HideUI()
{
    UIHidden = !UIHidden;
    
    auto PlayerController = Cast<APlayerController>(GetController());
    auto HUD = Cast<AGliderHUD>(PlayerController->GetHUD());
    HUD->SetVisible(!UIHidden);
}
