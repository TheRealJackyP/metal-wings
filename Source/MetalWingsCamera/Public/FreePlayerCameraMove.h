// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "Components/SceneComponent.h"
#include "FreePlayerCameraMove.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class METALWINGSCAMERA_API UFreePlayerCameraMove : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFreePlayerCameraMove();


private:
	class ACameraActor* Owner;
	FVector ForwardVector;
	FVector RightVector;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	//Moves the player's camera according to their input magnitude, starting at a minimum starting velocity and moving towards a max velocity based on the provided acceleration value.
	UFUNCTION(BlueprintNativeEvent, Category = "TopCategory", BlueprintCallable)
	void MoveCamera(FVector PlayerInputVector, float DeltaTime);
	virtual void MoveCamera_Implementation(FVector PlayerInputVector, float DeltaTime);

	//The acceleration per second to be applied to the camera movement.
	UPROPERTY(EditAnywhere)
	float MoveAcceeration;
	//The initial speed of the camera movement once started.
	UPROPERTY(EditAnywhere)
	float StartSpeed;
	//The maximum speed of the camera movement.
	UPROPERTY(EditAnywhere)
	float EndSpeed;

	//The current speed of the camera's movement.
	UPROPERTY(BlueprintReadOnly)
	float CurrentSpeed;
};
