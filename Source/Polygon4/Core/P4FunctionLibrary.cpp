//Copyright(c) 2019 Dmitry Yakimenko

#include "P4FunctionLibrary.h"

#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"


//For UE4 Profiler ~ Stat
//DECLARE_CYCLE_STAT(TEXT(" ~ Get Transform Component"), STAT_GetTransformComponent, STATGROUP_);

UP4FunctionLibrary::UP4FunctionLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// Get Component's world space transform from BodyInstance as its valid during physics sub-stepping
FTransform UP4FunctionLibrary::GetTransformComponent(USceneComponent *Target, FName InSocketName = NAME_None)
{
	//Gather stats
	//SCOPE_CYCLE_COUNTER(STAT_GetTransformComponent);

	if (Target == NULL) {
		return FTransform::Identity;
	}

	//Check if socket name is provided and store it's relative transform
	FTransform SocketRelativeTransform = FTransform::Identity;
	bool IsSocketRequested = false;
	if (InSocketName != NAME_None) {
		SocketRelativeTransform = Target->GetSocketTransform(InSocketName, ERelativeTransformSpace::RTS_Component);
		//Check if socket transform was found. GetSocketTransform() return transform of the parent when socket is not found.
		if (SocketRelativeTransform.Equals(Target->GetRelativeTransform()) == false) {
			//UE_LOG(LogTemp, Warning, TEXT("valid socket requested"));
			IsSocketRequested = true;
		}
	}

	//Check if component has a valid physics body to retrieve world transform. GetBodyInstanceTransform() returns parent transform if component's physics body is invalid.
	FTransform currentTransform = GetBodyInstanceTransform(Target);
	FTransform ParentTransform = GetBodyInstanceTransform(Target->GetAttachParent());
	if ((currentTransform.Equals(ParentTransform) == false) && (currentTransform.Equals(FTransform::Identity) == false)) {
		//UE_LOG(LogTemp, Warning, TEXT("Debug->return1"));
		return (IsSocketRequested ? SocketRelativeTransform * currentTransform : currentTransform);
	}

	//At this point we know that target component doesn't have valid physics body.
	//We need to find a parent in a chain, with valid physics body and accumulate all relative transforms leading to it.
	FTransform relativeTransform = Target->GetRelativeTransform();
	USceneComponent *currentScene = Target;

	while (true) {
		currentScene = currentScene->GetAttachParent();
		//We reached the top of hierarchy or current object simulates physics and therefore doesn't have a parent, as such objects are detached from the rest.
		if (currentScene == NULL) {
			//UE_LOG(LogTemp, Warning, TEXT("Debug->return2"));
			return (IsSocketRequested ? SocketRelativeTransform * Target->GetComponentTransform() : Target->GetComponentTransform());
		}
		currentTransform = GetBodyInstanceTransform(currentScene);
		ParentTransform = GetBodyInstanceTransform(currentScene->GetAttachParent());
		//We found a body in hierarchy which simulates physics. Now we can return final transform.
		if ((currentTransform.Equals(ParentTransform) == false) && (currentTransform.Equals(FTransform::Identity) == false)) {
			//UE_LOG(LogTemp, Warning, TEXT("Debug->return3"));
			return (IsSocketRequested ? SocketRelativeTransform * (relativeTransform * currentTransform) : relativeTransform * currentTransform);
		}
		relativeTransform *= currentScene->GetRelativeTransform();
	}
}

// Set Component transform on BodyInstance level, to be valid during physics sub-stepping
void UP4FunctionLibrary::SetTransformComponent(USceneComponent * Target, const FTransform& Transform, bool bTeleport)
{
	UPrimitiveComponent *primitiveComponent = Cast<UPrimitiveComponent>(Target);
	if (primitiveComponent == NULL) {
		return;
	}
	FBodyInstance *TargetBodyInstance = primitiveComponent->GetBodyInstance();

	TargetBodyInstance->SetBodyTransform(Transform, bTeleport ? ETeleportType::TeleportPhysics : ETeleportType::None);
}

// Get Actor transform from BodyInstance as its valid during physics sub-stepping
FTransform UP4FunctionLibrary::GetTransformActor(AActor * Actor)
{
	//UPrimitiveComponent* ActorRootComponent = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
	USceneComponent* ActorRootComponent = Actor->GetRootComponent();
	if (ActorRootComponent != NULL) {
		return GetBodyInstanceTransform(ActorRootComponent);
	}
	return FTransform::Identity;
}

// AddForce to a component using BodyInstance as its valid during physics sub-stepping
void UP4FunctionLibrary::AddForceComponent(UPrimitiveComponent * Target, const FVector & Force, bool bAccelChange)
{
	FBodyInstance* BodyInstance = GetBodyInstance(Target);
	FVector lForce = FVector::ZeroVector;
	if(bAccelChange)
	{
		lForce = Force;
	}
	else
	{
		lForce = Force * 100; //Convert to Newton m/s2
	}
	if (BodyInstance != NULL)
	{
		BodyInstance->AddForce(lForce, false, bAccelChange);
	}
}

// AddTorque to a component using BodyInstance as its valid during physics sub-stepping
void UP4FunctionLibrary::AddTorqueComponent(UPrimitiveComponent * Target, const FVector & Torque, bool bAccelChange)
{
	FBodyInstance* BodyInstance = GetBodyInstance(Target);

	if (BodyInstance != NULL) 
	{
		FVector lTorque = FMath::DegreesToRadians(Torque);
		BodyInstance->AddTorqueInRadians(lTorque, true, bAccelChange);
	}
}


// Return mesh component reference by finding it by name
UPrimitiveComponent* UP4FunctionLibrary::GetComponentReferenceByName(UActorComponent* Target, FString Name)
{
	if (IsValid(Target))
	{
		AActor* Owner = Target->GetOwner();
		TArray<UActorComponent*> FoundComponents = Owner->GetComponentsByClass(UPrimitiveComponent::StaticClass());

		UPrimitiveComponent* Result;

		for (int32 i = 0; i < FoundComponents.Num(); i++)
		{
			if (FoundComponents[i]->GetName() == Name)
			{
				Result = Cast<UPrimitiveComponent>(FoundComponents[i]);
				if (IsValid(Result))
				{
					return Result;
				}
			}
		}
	}
	return nullptr;
}


float UP4FunctionLibrary::Stabilizer(const float DT,const float CurrentValue, const float MinValue, const float MaxValue, const float MinForce, const float ForceMultiplier, const float Damping, const float OldValue)
{
	float lDelta;
	float lComressionRatio;
	float lForce;
	float lDamping;

	lDelta = FMath::Clamp((MaxValue - CurrentValue) / (MaxValue - MinValue), 0.0f, MaxValue);
	lComressionRatio = FMath::Clamp((CurrentValue - 1.0f) / MaxValue, 0.0f, 1.0f);

	if (lDelta > 1.0f)
	{
		lForce = FMath::Lerp(MinForce * ForceMultiplier, MinForce, lComressionRatio);
	}
	else
	{
		lForce = MinForce * lDelta;
	}

	lDamping = FMath::Clamp(Damping * ((CurrentValue - OldValue) / DT), 0.0f, lForce);
	
	return lForce - lDamping;
}

FVector UP4FunctionLibrary::CalculateAirDragForce(UPrimitiveComponent* Target, const FVector AirDragCoef, const float AreaOfDrag, const float AirDensity)
{
	if (IsValid(Target))
	{
		FTransform lTransform = GetTransformComponent(Target, NAME_None);
		FVector lLinearVelocity = Target->GetPhysicsLinearVelocity();
		FVector lInverseLinearVelocity = UKismetMathLibrary::InverseTransformDirection(lTransform, lLinearVelocity);
		FVector lLinearVelocityABS = lInverseLinearVelocity.GetAbs() * (-lInverseLinearVelocity);

		FVector lAirDragForce = (AirDragCoef * (AreaOfDrag / 100.f) * lLinearVelocityABS * AirDensity) / 2;

		return UKismetMathLibrary::TransformDirection(lTransform, lAirDragForce / 100.f);
	}
	return FVector::ZeroVector;
}

FVector UP4FunctionLibrary::CalculateFrictionForce(UPrimitiveComponent* Target, const float DT, const FVector ImpactNormal, const float MuCoefficient)
{
	if (IsValid(Target))
	{
		float Gravity = 980.f;
		float Mass = Target->GetMass();
		FTransform lTransform = GetTransformComponent(Target, NAME_None);
		FVector LinearVelocity = Target->GetPhysicsLinearVelocityAtPoint(lTransform.GetLocation(), NAME_None);
		FVector ProjectVectorPlane = -FVector::VectorPlaneProject(LinearVelocity, ImpactNormal);
		FVector lFrictionForce = (((ProjectVectorPlane * Mass) / DT) * MuCoefficient);

		lFrictionForce.X = FMath::Clamp(lFrictionForce.X, -Gravity, Gravity);
		lFrictionForce.Y = FMath::Clamp(lFrictionForce.Y, -Gravity, Gravity);
		lFrictionForce.Z = FMath::Clamp(lFrictionForce.Z, -Gravity, Gravity);

		return lFrictionForce;
	}
	return FVector::ZeroVector;
}

// Get instance of physics body from component
FBodyInstance * UP4FunctionLibrary::GetBodyInstance(UPrimitiveComponent * PrimitiveComponent)
{
	if (PrimitiveComponent == NULL) {
		return NULL;
	}
	return PrimitiveComponent->GetBodyInstance();
}

// Get transform from physics body of component
FTransform UP4FunctionLibrary::GetBodyInstanceTransform(USceneComponent *SceneComponent) {
	UPrimitiveComponent *primitiveComponent = Cast<UPrimitiveComponent>(SceneComponent);
	if (primitiveComponent == NULL) {
		return FTransform::Identity;
	}
	return primitiveComponent->GetBodyInstance()->GetUnrealWorldTransform();
}
