// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FFreeCameraConfigDataRow.h"
#include "Abilities/GameplayAbility.h"
#include "MouseMoveFreeCamera.generated.h"

/**
 * @brief A gameplay ability representing edge-scrolling camera movement based on the player's mouse position. Only defines methods for calculating the player's intended mouse movement. The actual details of what should happen with this value is left to the blueprint layer. 
 */
UCLASS()
class METALWINGSCAMERA_API UMouseMoveFreeCamera : public UGameplayAbility
{
	GENERATED_BODY()
public:
	/**
	 * @brief Calculates the FVector2D that corresponds to the movement the player is indicating based on how far their mouse cursor is placed on the screen relative to the nearest edge of the dead zone.
	 * @return A signed normalized FVector2D of the player's indicated free camera movement input. 
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "MouseMoveFreeCamera",
		BlueprintCallable)
	FVector2D CalculateMouseMovement();
	virtual FVector2D CalculateMouseMovement_Implementation();

	/**
	 * @brief Handles ability initialization before actually activating. Updates TargetController if needed as well as the screen resolution. Also performs validation to ensure that there actually is an owner of this ability and that the InputThreshold is non-zero along both axes.
	 * @param Handle The Handle of the ability that is about to be activated.
	 * @param ActorInfo The information about the actor owning the ability that is about to be activated.
	 * @param ActivationInfo The activation info for the ability that is about to be activated.
	 * @param OnGameplayAbilityEndedDelegate A delegate for callbacks when the gameplay ability is ended.
	 * @param TriggerEventData The event data that accompanies the triggering activation of this ability.
	 */
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle,
	                         const FGameplayAbilityActorInfo* ActorInfo,
	                         const FGameplayAbilityActivationInfo ActivationInfo,
	                         FOnGameplayAbilityEnded::FDelegate*
	                         OnGameplayAbilityEndedDelegate,
	                         const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	/**
	 * @brief Updates the internal values for the current screen resolution. Solely exists to match delegate callback for when the resolution changes.
	 */
	void UpdateCurrentResolution(FViewport*, uint32);
	/**
	 * @brief Updates the internal values for the current screen resolution.
	 */
	UFUNCTION()
	virtual void UpdateCurrentResolution();

	/**
	 * @brief The normalized size of the border along each axis that will be interpreted as non-zero free camera movement input. Values must be (0, .5f). 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "MouseMoveFreeCamera")
	FVector2D MovementThreshold;
	/**
	 * @brief The current screen resolution in pixels.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
		Category = "MouseMoveFreeCamera")
	FIntPoint CurrentResolution;

	UFUNCTION(BlueprintNativeEvent, Category = "FreeCameraMovementComponent",
		BlueprintCallable)
	void InitializeFromConfig(FFreeCameraConfigDataRow ConfigData);
	/**
	 * @brief A BP overridable method for initializing this component from a row of configuration data.
	 * @param ConfigData The row of configuration data containing the values to initialize this component with.
	 */
	virtual void InitializeFromConfig_Implementation(FFreeCameraConfigDataRow ConfigData);

private:
	/**
	 * @brief A shorthand for a reference to the owning player controller for this ability.
	 */
	APlayerController* TargetController;

	/**
	 * @brief Converts the values for the absolute mouse positions into normalized values per axis.
	 * @param ScreenPositionX The current X-value for the pixel screen position of the player's cursor. 
	 * @param ScreenPositionY The current Y-value for the pixel screen position of the player's cursor.
	 * @return A vector representing the normalized position of the player's mouse on the X and Y axes.
	 */
	FVector2D NormalizeMousePositions(int ScreenPositionX,
	                                  int ScreenPositionY) const;
	/**
	 * @brief Converts a single axis's normalized screen position into the signed intended input value for that axis based on the provided movement threshold for that axis.
	 * @param NormalizedScreenPosition The normalized position of the player's cursor for the given axis.
	 * @param MovementThreshold The movement threshold value for the given axis.
	 * @return The player's signed intended input value for this axis.
	 */
	static float ConvertAxisMovement(float NormalizedScreenPosition,
	                                 float MovementThreshold);
	/**
	 * @brief A delegate handle to use for unsubscribing this event from the resolution changed callback while the ability is disabled.
	 */
	FDelegateHandle UnsubscribeHandle;
};
