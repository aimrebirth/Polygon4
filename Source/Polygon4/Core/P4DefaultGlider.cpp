//Copyright(c) 2019 Dmitry Yakimenko

#include "P4DefaultGlider.h"

#include "Engine/CollisionProfile.h"
#include "DisplayDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "P4GliderMovementComponent.h"

AP4DefaultGlider::AP4DefaultGlider(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bReplicateMovement = true;
	NetUpdateFrequency = 10.f;

	GliderPhysicsBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GliderPhysicsBody"));
	GliderPhysicsBody->SetCollisionProfileName(TEXT("GliderPhysicsBody"));
	GliderPhysicsBody->BodyInstance.bSimulatePhysics = true;
	GliderPhysicsBody->BodyInstance.bNotifyRigidBodyCollision = true;
	GliderPhysicsBody->BodyInstance.bUseCCD = true;
	GliderPhysicsBody->SetMassOverrideInKg(NAME_None, 1000.f);
	GliderPhysicsBody->SetLinearDamping(0.f);
	GliderPhysicsBody->SetAngularDamping(0.f);
	GliderPhysicsBody->ScaleByMomentOfInertia(FVector(0.f, 0.f, 0.f));
	GliderPhysicsBody->SetGenerateOverlapEvents(true);
	GliderPhysicsBody->SetCanEverAffectNavigation(false);
	RootComponent = GliderPhysicsBody;


	GliderMovementComponent = CreateDefaultSubobject<UP4GliderMovementComponent>(TEXT("GliderMovementComponent"));
	GliderMovementComponent->UpdatedComponent = RootComponent;
}

// Called when the game starts or when spawned
void AP4DefaultGlider::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AP4DefaultGlider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AP4DefaultGlider::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
