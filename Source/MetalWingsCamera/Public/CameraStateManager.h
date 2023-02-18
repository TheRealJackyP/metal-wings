// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraStateManager.generated.h"

UENUM(BlueprintType)
enum class ECameraState : uint8
{
	FREE_PLAYER,
	FIXED_PLAYER,
};

UDELEGATE(Category = "CameraStateManager", BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCameraStateEventDelegate, ECameraState,
                                             PreviousState, ECameraState, NextState);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class METALWINGSCAMERA_API UCameraStateManager : public UActorComponent
{
	GENERATED_BODY()

public:
	//UDELEGATE(BlueprintNativeEvent, Category = "Camera State Manager", BlueprintCallable)

	// Sets default values for this component's properties
	UCameraStateManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintNativeEvent, Category = "CameraStateManager", BlueprintCallable)
	void UpdateCameraState(ECameraState NextCameraState);
	virtual void UpdateCameraState_Implementation(ECameraState NextCameraState);

	UPROPERTY(BlueprintAssignable, Category = "CameraStateManager")
	FCameraStateEventDelegate OnCameraStateChangedEvent;

	UPROPERTY(BlueprintReadOnly)
	ECameraState CurrentCameraState;
};
