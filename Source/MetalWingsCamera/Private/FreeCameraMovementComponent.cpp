// Fill out your copyright notice in the Description page of Project Settings.


#include "FreeCameraMovementComponent.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"

FVector UFreeCameraMovementComponent::ComputeMoveDelta(const FVector& InVelocity,
                                                       float DeltaTime)
{
	if (FreeCameraState == EFreeCameraState::INSTANT_STOP)
	{
		Velocity = FVector(0);
	}
	else
	{
		Velocity = ComputeAcceleration(InVelocity, DeltaTime);
	}
	return Velocity * DeltaTime;
}

FVector UFreeCameraMovementComponent::ComputeAcceleration(const FVector& InVelocity,
	float DeltaTime) const
{
	switch (FreeCameraState)
	{
	case EFreeCameraState::ACTIVE:
		{
			return ComputeActiveAcceleration(InVelocity, DeltaTime);
		}
	case EFreeCameraState::BRAKING_NO_INTERRUPT:
		{
			return ComputeBrakingAcceleration(InVelocity, DeltaTime);
		}
	case EFreeCameraState::BRAKING:
		{
			return ComputeBrakingAcceleration(InVelocity, DeltaTime);
		}
	default:
		{
			return FVector(0);
		}
	}
}

FVector UFreeCameraMovementComponent::ComputeActiveAcceleration(
	const FVector& InVelocity, float DeltaTime) const
{
	const FVector TargetVelocity = (ForwardVector * PlayerInputVector.Y * MaxSpeed) +
		RightVector * PlayerInputVector.X * MaxSpeed;
	FVector AccelerationDirection = (TargetVelocity - InVelocity);
	AccelerationDirection.Normalize();
	const FVector NextVelocity = InVelocity + AccelerationDirection * MoveAcceleration *
		DeltaTime;
	return NextVelocity;
}

FVector UFreeCameraMovementComponent::ComputeBrakingAcceleration(
	const FVector& InVelocity, float DeltaTime) const
{
	const FVector NextVelocity = InVelocity - (InVelocity.GetSafeNormal() *
		BrakingAcceleration * DeltaTime);
	return NextVelocity.IsNearlyZero() ? FVector(0) : NextVelocity;
}

void UFreeCameraMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                 FActorComponentTickFunction*
                                                 ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	const FVector Location = TargetCameraComponent->GetComponentLocation() +
		ComputeMoveDelta(Velocity, DeltaTime);
	FString string = Location.ToCompactString();
	UE_LOG(LogTemp, Warning, TEXT("%s"), &string);
	TargetCameraComponent->SetWorldLocation(Location);
}

void UFreeCameraMovementComponent::StopMovement_Implementation(bool ShouldBrake,
	bool NoInterrupt)
{
	if (!ShouldBrake)
	{
		FreeCameraState = EFreeCameraState::INSTANT_STOP;
		return;
	}
	if (NoInterrupt)
	{
		FreeCameraState = EFreeCameraState::BRAKING_NO_INTERRUPT;
		return;
	}

	FreeCameraState = EFreeCameraState::BRAKING;
}

void UFreeCameraMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	FreeCameraState = EFreeCameraState::ACTIVE;
	TargetCameraComponent = Cast<ACameraActor>(this->GetOwner())->GetCameraComponent();
}
