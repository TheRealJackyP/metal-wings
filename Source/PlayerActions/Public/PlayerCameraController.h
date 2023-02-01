// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCameraController.generated.h"

/**
 * 
 */
UCLASS()
class PLAYERACTIONS_API APlayerCameraController : public APlayerController
{
	GENERATED_BODY()

public:
	APlayerCameraController();
	UPROPERTY(VisibleAnywhere)
	float MoveScreenXThreshold;

protected:
	void BeginPlay();

public:
	void Tick(float DeltaTime);

};
