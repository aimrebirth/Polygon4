//Copyright(c) 2019 Dmitry Yakimenko

#pragma once

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsSettings.h"
#include "P4FunctionLibrary.generated.h"

UCLASS()
class POLYGON4_API UP4FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:

	/**
	*	Get world-space component's transform. BodyInstance is used to retrieve transform, its up-to date withPhysics sub-stepping.
	*	@param Target	Component's reference to get the transform for
	*	@param InSocketName	Optional socket name parameter. Will return socket transform, if socket not found returns component's transform
	*	@return			Component's or socket's transform in world space
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = " Get Transform -Comp"), Category = "Physics sub-stepping")
		static FTransform GetTransformComponent(USceneComponent * Target, FName InSocketName);

	/**
	*	Sets component's BodyInstance transform. To be used during sub-stepping to move/rotate PhysX bodies directly.
	*	@param Target	Component's reference to set transform for
	*	@param bTeleport		No velocity will be inferred, object is moved in instant
	*	@param Transform	New transform to be set
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = " Set Transform -Comp"), Category = "Physics sub-stepping")
		static void SetTransformComponent(USceneComponent * Target, const FTransform& Transform, bool bTeleport);

	/**
	*	Get world-space actor's root component transform. BodyInstance is used to retrieve transform, its up-to date withPhysics sub-stepping.
	*	@param Actor	Actor's reference to get the transform for
	*	@return			Actor's root component transform in world space
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = " Get Transform -Actor"), Category = "Physics sub-stepping")
		static FTransform GetTransformActor(AActor* Actor);

	/**
	*	Add a force to a component. Can apply force duringPhysics sub-steps.
	*	@param Target		Component's reference to apply force to
	*	@param Force		Force vector to apply. Magnitude indicates strength of force.
	*	@param bAccelChange If true, Force is taken as a change in acceleration instead of a physical force (i.e. mass will have no affect).
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = " Add Force -Comp"), Category = "Physics sub-stepping")
		static void AddForceComponent(UPrimitiveComponent* Target, const FVector& Force, bool bAccelChange);

	/**
	*	Add a torque to a component. Can apply torque duringPhysics sub-steps.
	*	@param Target		Component's reference to apply torque to
	*	@param Torque		Force vector to apply. Magnitude indicates strength of force.
	*	@param bAccelChange If true, Force is taken as a change in acceleration instead of a physical force (i.e. mass will have no affect).
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = " Add Torque -Comp"), Category = "Physics sub-stepping")
		static void AddTorqueComponent(UPrimitiveComponent* Target, const FVector& Torque, bool bAccelChange);

	/**
	*	Returns reference to component by the name of the component.
	*	@param Target		Components reference that requests reference to component
	*	@param Name			Name of the component that needs to be found
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = " Get Component Reference by Name"), Category = " Utility")
		static UPrimitiveComponent* GetComponentReferenceByName(UActorComponent* Target, FString Name);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = " CalculateAirDragForce "), Category = " Utility")
		static FVector CalculateAirDragForce(UPrimitiveComponent* Target, const FVector AirDragCoef, const float AreaOfDrag, const float AirDensity);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = " CalculateFrictionForce "), Category = " Utility")
		static FVector CalculateFrictionForce (UPrimitiveComponent* Target, const float DT, const FVector ImpactNormal, const float MuCoefficient);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = " Stabilizer "), Category = " Utility")
		static float Stabilizer(const float DT, const float CurrentValue, const float MinValue, const float MaxValue, const float MinForce, const float ForceMultiplier, const float Damping, const float OldValue);//,float& Force);

private:
	static FBodyInstance* GetBodyInstance(UPrimitiveComponent* PrimitiveComponent);
	static FTransform GetBodyInstanceTransform(USceneComponent *SceneComponent);
};