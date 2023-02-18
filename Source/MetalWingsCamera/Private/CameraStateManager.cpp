// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraStateManager.h"

// Sets default values for this component's properties
UCameraStateManager::UCameraStateManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCameraStateManager::BeginPlay()
{
	Super::BeginPlay();
	CurrentCameraState = ECameraState::FREE_PLAYER;
	// ...
}


// Called every frame
void UCameraStateManager::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCameraStateManager::UpdateCameraState_Implementation(ECameraState NextCameraState)
{
	OnCameraStateChangedEvent.Broadcast(CurrentCameraState, NextCameraState);
	CurrentCameraState = NextCameraState;
}
