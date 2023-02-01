// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCameraController.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"

APlayerCameraController::APlayerCameraController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerCameraController::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCameraController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float mouseX;
	float mouseY;
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetMousePosition(mouseX, mouseY);
	UE_LOG(LogTemp, Warning, TEXT("Mouse Location: %f, %f"), mouseX, mouseY);
}