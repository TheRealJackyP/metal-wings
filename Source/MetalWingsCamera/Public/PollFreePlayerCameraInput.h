#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "FreeCameraMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "PollFreePlayerCameraInput.generated.h"

class UAbilitySystemComponent;


/**
 * @brief An ability task that will poll the input values from a specified Input Action and inject them into a Free Camera Movement component. Exits when a particular Gameplay Event is raised.
 */
UCLASS()
class METALWINGSCAMERA_API
	UPollFreePlayerCameraInput : public UAbilityTask_WaitGameplayEvent
{
private:
	GENERATED_BODY()
public:
	/**
	 * @brief Updates the values for the player's Input Action. Also checks for the targeted gameplay event.
	 * @param DeltaTime The amount of time that elapsed since the last tick.
	 */
	virtual void TickTask(float DeltaTime) override;

	/**
	 * @brief Blueprint constructor for using this task.
	 * @param OwningAbility The ability that owns this task.
	 * @param EventTag The event tag to watch for before exiting.
	 * @param TargetActionBinding The Input Action to bind to.
	 * @param PollOptionalExternalTarget Look for tags on external actors.
	 * @param PollOnlyTriggerOnce Will continue to wait for the tag while this is false.
	 * @param PollOnlyMatchExact Will match sub-tags if true.
	 * @param TargetCameraComponent The Camera Component that will receive injected values.
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks|Custom",
		meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility",
			BlueprintInternalUseOnly = "TRUE"))
	static UPollFreePlayerCameraInput* PollFreePlayerCameraInput(
		UGameplayAbility* OwningAbility, FGameplayTag EventTag,
		UInputAction* TargetActionBinding, AActor* PollOptionalExternalTarget = nullptr,
		bool PollOnlyTriggerOnce = false, bool PollOnlyMatchExact = true,
		UFreeCameraMovementComponent* TargetCameraComponent = nullptr);

	/**
	 * @brief The Camera Component that will receive injected values. Maybe should make this readonly and private?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "FreeCameraMovementComponent")
	UFreeCameraMovementComponent* TargetCameraComponent;
	/**
	 * @brief A reference to the actual value binding that contains the input values.
	 */
	FEnhancedInputActionValueBinding* TargetBinding;

private:
	/**
	 * @brief A helper function when constructing an instance of this class to use the Input Action Value binding from the originally provided Input Action.
	 * @param TargetInstance A reference to the instance that is currently undergoing initialization. Unsure whether this could simply be replaced with references to self/this since the constructor it gets called from is a static method. 
	 * @param TargetAction A reference to the Input Action that will provide the values to drive the camera movement.
	 * @return A reference to the value binding of the provided Input Action.
	 */
	FEnhancedInputActionValueBinding* GetTargetValueBinding(
		const UPollFreePlayerCameraInput* TargetInstance,
		const UInputAction* TargetAction) const;
};
