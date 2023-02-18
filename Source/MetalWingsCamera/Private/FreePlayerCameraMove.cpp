// Fill out your copyright notice in the Description page of Project Settings.


#include "FreePlayerCameraMove.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UFreePlayerCameraMove::UFreePlayerCameraMove()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	CurrentSpeed = 0;
}


// Called when the game starts
void UFreePlayerCameraMove::BeginPlay()
{
	Super::BeginPlay();
	CurrentSpeed = 0;
	Owner = Cast<class ACameraActor>(this->GetOwner());
	class UCameraComponent* cameraComponent = Owner->GetCameraComponent();
	ForwardVector = cameraComponent->GetForwardVector();
	ForwardVector.Z = 0;
	RightVector = cameraComponent->GetRightVector();
}


// Called every frame
void UFreePlayerCameraMove::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFreePlayerCameraMove::MoveCamera_Implementation(
	FVector PlayerInputVector, float DeltaTime)
{
	FVector currentLocation = Owner->GetActorLocation();
	FVector targetDirection = PlayerInputVector.X * RightVector + PlayerInputVector.Y *
		ForwardVector;
	targetDirection.Normalize();
	currentLocation += targetDirection;
	Owner->SetActorLocation(currentLocation);
}
