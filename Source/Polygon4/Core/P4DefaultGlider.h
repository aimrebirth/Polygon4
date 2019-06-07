//Copyright(c) 2019 Dmitry Yakimenko

#pragma once

#include "GameFramework/Pawn.h"

#include "P4DefaultGlider.generated.h"

/**
 * Glider class with implemented custom physics
 */
UCLASS(abstract, Blueprintable, BlueprintType)
class POLYGON4_API AP4DefaultGlider : public APawn
{
	GENERATED_UCLASS_BODY()
public:
	// Sets default values for this pawn's properties
	AP4DefaultGlider();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

		//////////////////////////////////////////////////////////////////////////
	// Glider setup

	UPROPERTY(Category = "GliderPhysicsBody", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* GliderPhysicsBody;

	UPROPERTY(Category = "Glider Movement Component", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UP4GliderMovementComponent* GliderMovementComponent;
};