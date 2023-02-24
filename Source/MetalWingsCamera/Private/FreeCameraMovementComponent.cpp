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
	const FVector TargetVelocity = ((ForwardVector * PlayerInputVector.Y) + (RightVector * PlayerInputVector.X)) * MaxSpeed;
	if(MoveAcceleration == 0)
	{
		return TargetVelocity;
	}
	if(TargetVelocity.IsNearlyZero())
	{
		return ComputeBrakingAcceleration(InVelocity, DeltaTime);
	}
	FVector AccelerationDirection = (TargetVelocity - InVelocity);
	AccelerationDirection.Normalize();
	const FVector NextVelocity = InVelocity + AccelerationDirection * MoveAcceleration *
		DeltaTime;
	return NextVelocity.GetClampedToSize(0, TargetVelocity.IsNearlyZero() ? 0 : TargetVelocity.Length());
}

FVector UFreeCameraMovementComponent::ComputeBrakingAcceleration(
	const FVector& InVelocity, float DeltaTime) const
{
	if(BrakingAcceleration == 0)
	{
		return FVector(0);
	}
	FVector NextVelocity = InVelocity - (InVelocity.GetSafeNormal() *
		BrakingAcceleration * DeltaTime);
	
	if(InVelocity.Length() < BrakingAcceleration * DeltaTime)
	{
		NextVelocity = FVector(0); 
	}
	
	return NextVelocity.IsNearlyZero() ? FVector(0) : NextVelocity;
}

void UFreeCameraMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                 FActorComponentTickFunction*
                                                 ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	const FVector Location = TargetCameraComponent->GetComponentLocation() +
		ComputeMoveDelta(Velocity, DeltaTime);
	//FString string = Location.ToCompactString();
	TargetCameraComponent->SetWorldLocation(Location, true);
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
	ForwardVector = TargetCameraComponent->GetForwardVector();
	ForwardVector.Z = 0;
	ForwardVector.Normalize();
	RightVector = TargetCameraComponent->GetRightVector();
	RightVector.Z = 0;
	RightVector.Normalize();
}
