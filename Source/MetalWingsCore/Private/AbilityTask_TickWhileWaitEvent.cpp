// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask_TickWhileWaitEvent.h"
UAbilityTask_TickWhileWaitEvent::UAbilityTask_TickWhileWaitEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
}

UAbilityTask_TickWhileWaitEvent* UAbilityTask_TickWhileWaitEvent::TickWhileWaitEvent(
	UGameplayAbility* OwningAbility, FGameplayTag EventTag,
	AActor* TickOptionalExternalTarget, bool TickOnlyTriggerOnce, bool TickOnlyMatchExact)
{
	UAbilityTask_TickWhileWaitEvent* MyObj = NewAbilityTask<UAbilityTask_TickWhileWaitEvent>(OwningAbility);
	MyObj->Tag = EventTag;
	MyObj->SetExternalTarget(TickOptionalExternalTarget);
	MyObj->OnlyTriggerOnce = TickOnlyTriggerOnce;
	MyObj->OnlyMatchExact = TickOnlyMatchExact;
	return MyObj;
}

void UAbilityTask_TickWhileWaitEvent::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	Tick.Broadcast(FGameplayEventData());
}
