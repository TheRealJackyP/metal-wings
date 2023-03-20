// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilityTask_TickWhileWaitEvent.generated.h"

/**
 * 
 */
UCLASS()
class METALWINGSCORE_API
	UAbilityTask_TickWhileWaitEvent : public UAbilityTask_WaitGameplayEvent
{
	GENERATED_BODY()
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks|Custom",
		meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility",
			BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_TickWhileWaitEvent* TickWhileWaitEvent(
		UGameplayAbility* OwningAbility, FGameplayTag EventTag,
		AActor* TickOptionalExternalTarget = nullptr, bool TickOnlyTriggerOnce = false,
		bool TickOnlyMatchExact = true);
public:
	UPROPERTY(BlueprintAssignable)
	FWaitGameplayEventDelegate Tick;

	virtual void TickTask(float DeltaTime) override;
};
