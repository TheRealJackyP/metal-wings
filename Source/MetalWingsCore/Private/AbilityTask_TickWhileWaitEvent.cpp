// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask_TickWhileWaitEvent.h"

UAbilityTask_TickWhileWaitEvent* UAbilityTask_TickWhileWaitEvent::TickWhileWaitEvent(
	UGameplayAbility* OwningAbility, FGameplayTag EventTag,
	AActor* TickOptionalExternalTarget, bool TickOnlyTriggerOnce, bool TickOnlyMatchExact)
{
	UAbilityTask_TickWhileWaitEvent* Instance = Cast<UAbilityTask_TickWhileWaitEvent>(
		WaitGameplayEvent(OwningAbility, EventTag, TickOptionalExternalTarget, TickOnlyTriggerOnce, TickOnlyMatchExact));
	return Instance;
}

void UAbilityTask_TickWhileWaitEvent::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	Tick.Broadcast(FGameplayEventData());
}
