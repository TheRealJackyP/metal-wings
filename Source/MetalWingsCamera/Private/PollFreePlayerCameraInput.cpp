#include "PollFreePlayerCameraInput.h"

#include "GameFramework/GameSession.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"

void UPollFreePlayerCameraInput::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	TargetCameraComponent->PlayerInputVector = TargetBinding->GetValue().Get<
		FVector2d>();
}


UPollFreePlayerCameraInput* UPollFreePlayerCameraInput::PollFreePlayerCameraInput(
	UGameplayAbility* OwningAbility, const FGameplayTag EventTag,
	UInputAction* TargetActionBinding,
	AActor* PollOptionalExternalTarget, const bool PollOnlyTriggerOnce,
	const bool PollOnlyMatchExact, UFreeCameraMovementComponent* TargetCameraComponent)
{
	UPollFreePlayerCameraInput* MyObj = NewAbilityTask<UPollFreePlayerCameraInput>(
		OwningAbility);
	MyObj->Tag = EventTag;
	MyObj->SetExternalTarget(PollOptionalExternalTarget);
	MyObj->OnlyTriggerOnce = PollOnlyTriggerOnce;
	MyObj->OnlyMatchExact = PollOnlyMatchExact;
	MyObj->TargetCameraComponent = TargetCameraComponent;
	MyObj->TargetBinding = MyObj->GetTargetValueBinding(MyObj, TargetActionBinding);
	MyObj->bTickingTask = true;
	return MyObj;
}

FEnhancedInputActionValueBinding* UPollFreePlayerCameraInput::GetTargetValueBinding(
	const UPollFreePlayerCameraInput* TargetInstance,
	const UInputAction* TargetAction) const
{
	const APlayerController* TargetPlayerController = Cast<APlayerController>(
		TargetInstance->TaskOwner->GetGameplayTaskOwner(TargetInstance));

	UEnhancedInputComponent* InputComponent = Cast<UEnhancedInputComponent>(
		TargetPlayerController->InputComponent);
	return &InputComponent->BindActionValue(TargetAction);
}
