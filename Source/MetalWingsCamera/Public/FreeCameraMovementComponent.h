#pragma once

#include "CoreMinimal.h"
#include "FFreeCameraConfigDataRow.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/MovementComponent.h"
#include "FreeCameraMovementComponent.generated.h"


/**
 * @brief An enum representing the various states that the free camera movement component can be in.
 */
UENUM(BlueprintType)
enum class EFreeCameraState : uint8
{
	ACTIVE,
	BRAKING_NO_INTERRUPT,
	BRAKING,
	INSTANT_STOP
};

/**
 * @brief A component to be attached to a Camera Actor that provides configurable free camera movement for a MOBA/RTS style camera with fixed rotation based on a provided input vector. Will accelerate to a target velocity represented by the magnitude of the provided input. If a player inputs .5 along the x-Axis, then the component accelerates to half of the maximum velocity. 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class METALWINGSCAMERA_API UFreeCameraMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

private:
	/**
	 * @brief The direction that represents forward from the camera's perspective.
	 */
	FVector ForwardVector;
	/**
	 * @brief The direction that represents right from the camera's perspective.
	 */
	FVector RightVector;
	/**
	 * @brief The current state of this component.
	 */
	EFreeCameraState FreeCameraState;
	/**
	 * @brief The Camera component whose transform will be modified by this component.
	 */
	UCameraComponent* TargetCameraComponent;

protected:
	/**
	 * @brief Updates the Velocity of the component and computes the amount and direction that the camera should move this tick. 
	 * @param InVelocity The Velocity of this component before applying acceleration for this tick. 
	 * @param DeltaTime The amount of time that has elapsed since the last tick.
	 * @return The amount and direction that the camera should move this tick.
	 */
	virtual FVector ComputeMoveDelta(const FVector& InVelocity, float DeltaTime);

	/**
	 * @brief Computes the velocity after applying acceleration for this tick and accounting for the current movement mode.
	 * @param InVelocity The Velocity of this component before applying acceleration for this tick. 
	 * @param DeltaTime The amount of time that has elapsed since the last tick.
	 * @return The new velocity after applying the acceleration for this tick.
	 */
	virtual FVector ComputeAcceleration(const FVector& InVelocity,
	                                    float DeltaTime);

	/**
	 * @brief Computes the velocity when normally applying acceleration for this tick. 
	 * @param InVelocity The Velocity of this component before applying acceleration for this tick. 
	 * @param DeltaTime The amount of time that has elapsed since the last tick.
	 * @return The velocity after applying acceleration for this tick, capped by the Maximum Speed.
	 */
	virtual FVector ComputeActiveAcceleration(const FVector& InVelocity,
	                                          float DeltaTime);
	/**
	 * @brief Computes the velocity when this component should be braking.
	 * @param InVelocity The Velocity of this component before applying acceleration for this tick. 
	 * @param DeltaTime The amount of time that has elapsed since the last tick.
	 * @return The velocity for this component when it should be braking.
	 */
	virtual FVector ComputeBrakingAcceleration(const FVector& InVelocity,
	                                           float DeltaTime);


public:
	/**
	 * @brief Updates the camera position based on the provided acceleration and player input values.
	 * @param DeltaTime The amount of time that has elapsed since the last tick.
	 * @param TickType The type of tick we wish to perform.
	 * @param ThisTickFunction Tick function that calls ConditionalTick
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintNativeEvent, Category = "FreeCameraMovementComponent",
		BlueprintCallable)
	void StopMovement(bool ShouldBrake, bool NoInterrupt);
	/**
	 * @brief A BP overridable method for stopping the movement of this component.
	 * @param ShouldBrake If true, will smoothly come to a stop. If false, will immediately stop.
	 * @param NoInterrupt If true, will continue braking and will ignore further player input. If false, will brake, but will stop braking if player input is received.
	 */
	virtual void StopMovement_Implementation(bool ShouldBrake, bool NoInterrupt);

	UFUNCTION(BlueprintNativeEvent, Category = "FreeCameraMovementComponent",
		BlueprintCallable)
	void InitializeFromConfig(FFreeCameraConfigDataRow ConfigData);
	/**
	 * @brief A BP overridable method for initializing this component from a row of configuration data.
	 * @param ConfigData The row of configuration data containing the values to initialize this component with. 
	 */
	virtual void InitializeFromConfig_Implementation(FFreeCameraConfigDataRow ConfigData);
	
	/**
	 * @brief On the first tick, initialize all private variables. 
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief Gets the maximum speed this component can reach.
	 * @return The maximum speed this component can reach.
	 */
	virtual float GetMaxSpeed() const override { return MaxSpeed; }
	/**
	 * @brief The magnitude of the acceleration that should be applied per second to this component's Velocity.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveAcceleration;
	/**
	 * @brief The magnitude of the acceleration that should be applied per second to this component's Velocity when braking.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakingAcceleration;

	/**
	 * @brief The maximum speed of the camera movement.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed;

	/**
	 * @brief The injected player's input vector.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "FreeCameraMovementComponent")
	FVector2D PlayerInputVector;

	/**
	 * @brief The minimum bounds that the camera can reach. The camera will not move beyond these.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector MinBounds;

	/**
	 * @brief The maximum bounds that the camera can reach. The camera will not move beyond these.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector MaxBounds;

	/**
	 * @brief Returns the current state of this component.
	 * @return The current state of this component.
	 */
	UFUNCTION(Category = "FreeCameraMovementComponent", BlueprintCallable)
	virtual EFreeCameraState GetFreeCameraState() const { return FreeCameraState; }
};
