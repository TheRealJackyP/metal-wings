// Fill out your copyright notice in the Description page of Project Settings.


#include "PollFreePlayerCameraInput.h"

#include "GameFramework/GameSession.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"

//UPollFreePlayerCameraInput::UPollFreePlayerCameraInput()
//{
//	PlayerInputVector =	FVector2D(0);
//}
void UPollFreePlayerCameraInput::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	//TaskOwner->GetGameplayTaskOwner(this);
	TargetCameraComponent->PlayerInputVector = input->GetBoundActionValue(TargetAction).Get<FVector2D>();
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *thing.ToString());
}



UPollFreePlayerCameraInput* UPollFreePlayerCameraInput::PollFreePlayerCameraInput(
	UGameplayAbility* OwningAbility, FGameplayTag EventTag, FString TargetActionBindingName,
	AActor* PollOptionalExternalTarget, bool PollOnlyTriggerOnce, bool PollOnlyMatchExact , UFreeCameraMovementComponent* TargetCameraComponent)
{
	UPollFreePlayerCameraInput* MyObj = NewAbilityTask<UPollFreePlayerCameraInput>(OwningAbility);
	MyObj->Tag = EventTag;
	MyObj->SetExternalTarget(PollOptionalExternalTarget);
	MyObj->OnlyTriggerOnce = PollOnlyTriggerOnce;
	MyObj->OnlyMatchExact = PollOnlyMatchExact;
	MyObj->TargetCameraComponent=TargetCameraComponent;
	//MyObj->TaskOwner->GetGameplayTaskOwner(MyObj)
	APlayerController* controller = Cast<APlayerController>(MyObj->TaskOwner->GetGameplayTaskOwner(MyObj));
	MyObj->input = Cast<UEnhancedInputComponent>(controller->InputComponent);
	auto bindings = MyObj->input->GetActionValueBindings();
	auto glambda = [TargetActionBindingName](FEnhancedInputActionValueBinding& item) { return item.GetAction()->GetName() == TargetActionBindingName; };
	MyObj->TargetBinding = bindings.FindByPredicate(glambda);
	MyObj->TargetAction = MyObj->TargetBinding->GetAction();
	
	MyObj->bTickingTask = true;
	return MyObj;
}
