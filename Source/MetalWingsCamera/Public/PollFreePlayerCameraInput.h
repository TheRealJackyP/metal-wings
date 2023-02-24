// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "FreeCameraMovementComponent.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "PollFreePlayerCameraInput.generated.h"

class UAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class METALWINGSCAMERA_API UPollFreePlayerCameraInput : public UAbilityTask_WaitGameplayEvent
{
private:
	GENERATED_BODY()
public:
//	UPollFreePlayerCameraInput();
	virtual void TickTask(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks|Custom", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UPollFreePlayerCameraInput* PollFreePlayerCameraInput(UGameplayAbility* OwningAbility, FGameplayTag EventTag, FString TargetActionBindingName, AActor* PollOptionalExternalTarget=nullptr, bool PollOnlyTriggerOnce=false, bool PollOnlyMatchExact = true, UFreeCameraMovementComponent* TargetCameraComponent = nullptr);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FreeCameraMovementComponent")
	UFreeCameraMovementComponent* TargetCameraComponent;

	FEnhancedInputActionValueBinding* TargetBinding;
	const UInputAction* TargetAction;

	UEnhancedInputComponent* input;
};
