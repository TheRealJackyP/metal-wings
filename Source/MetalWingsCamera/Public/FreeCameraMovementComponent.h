// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/MovementComponent.h"
#include "FreeCameraMovementComponent.generated.h"


UENUM(BlueprintType)
enum class EFreeCameraState : uint8
{
	ACTIVE,
	BRAKING_NO_INTERRUPT,
	BRAKING,
	INSTANT_STOP
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class METALWINGSCAMERA_API UFreeCameraMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

private:
	FVector ForwardVector;
	FVector RightVector;
	EFreeCameraState FreeCameraState;
	float AccelerationDeadZone = .01f;
	UCameraComponent* TargetCameraComponent;
protected:
	/** Compute the distance we should move in the given time, at a given a velocity. */
	virtual FVector ComputeMoveDelta(const FVector& InVelocity, float DeltaTime);

	/** Compute the acceleration that will be applied */
	virtual FVector ComputeAcceleration(const FVector& InVelocity,
	                                    float DeltaTime) const;

	virtual FVector ComputeActiveAcceleration(const FVector& InVelocity,
	                                          float DeltaTime) const;
	virtual FVector ComputeBrakingAcceleration(const FVector& InVelocity,
	                                           float DeltaTime) const;


public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintNativeEvent, Category = "FreeCameraMovementComponent",
		BlueprintCallable)
	void StopMovement(bool ShouldBrake, bool NoInterrupt);
	virtual void StopMovement_Implementation(bool ShouldBrake, bool NoInterrupt);
	virtual void BeginPlay() override;

	virtual float GetMaxSpeed() const override { return MaxSpeed; }
	UPROPERTY(EditAnywhere)
	float MoveAcceleration;
	UPROPERTY(EditAnywhere)
	float BrakingAcceleration;

	//The maximum speed of the camera movement.
	UPROPERTY(EditAnywhere)
	float MaxSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FreeCameraMovementComponent")
	FVector2D PlayerInputVector;

	UPROPERTY(EditAnywhere)
	FVector MinBounds;
	UPROPERTY(EditAnywhere)
	FVector MaxBounds;

	UFUNCTION(Category = "FreeCameraMovementComponent", BlueprintCallable)
	virtual EFreeCameraState GetFreeCameraState() const { return FreeCameraState; }
};
