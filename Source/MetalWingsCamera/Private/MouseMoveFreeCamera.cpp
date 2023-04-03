// Fill out your copyright notice in the Description page of Project Settings.


#include "MouseMoveFreeCamera.h"
#include "MetalWingsCore/Public/ViewportUtilities.h"

FVector2D UMouseMoveFreeCamera::CalculateMouseMovement_Implementation()
{
	float RawX = 0;
	float RawY = 0;
	TargetController->GetMousePosition(RawX, RawY);
	const auto NormalizedInput = NormalizeMousePositions(RawX, RawY);
	return FVector2D(ConvertAxisMovement(NormalizedInput.X, MovementThreshold.X),
	                 -ConvertAxisMovement(NormalizedInput.Y, MovementThreshold.Y));
}

void UMouseMoveFreeCamera::InitializeFromConfig_Implementation(
	FFreeCameraConfigDataRow ConfigData)
{
	MovementThreshold = ConfigData.MouseMovementThreshold;
}

void UMouseMoveFreeCamera::PreActivate(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo
                                       ActivationInfo,
                                       FOnGameplayAbilityEnded::FDelegate*
                                       OnGameplayAbilityEndedDelegate,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo,
	                   OnGameplayAbilityEndedDelegate,
	                   TriggerEventData);

	UpdateCurrentResolution();
	UnsubscribeHandle = FViewport::ViewportResizedEvent.AddUObject(
		this, &UMouseMoveFreeCamera::UpdateCurrentResolution);
	checkf(
		FFloatRange::Exclusive(0, .5f).Contains(MovementThreshold.X) &&
		FFloatRange::Exclusive(0, .5f).Contains(MovementThreshold.Y),
		TEXT("Invalid ControlThreshold value: %s."),
		*MovementThreshold.ToString());
	checkf(ActorInfo->OwnerActor.IsValid(), TEXT(
		       "Owner Actor reference is invalid."
	       ))

	TargetController = Cast<APlayerController>(ActorInfo->OwnerActor.Get());
}

void UMouseMoveFreeCamera::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility,
	                  bWasCancelled);
	FViewport::ViewportResizedEvent.Remove(
		UnsubscribeHandle);
}

void UMouseMoveFreeCamera::UpdateCurrentResolution(FViewport*, uint32)
{
	UpdateCurrentResolution();
}


void UMouseMoveFreeCamera::UpdateCurrentResolution()
{
	const FVector2D ViewportSize = FViewportUtilities::GetGameViewportSize();
	CurrentResolution = FIntPoint(ViewportSize.X, ViewportSize.Y);
}

FVector2D UMouseMoveFreeCamera::NormalizeMousePositions(const int ScreenPositionX,
                                                        const int ScreenPositionY)
const
{
	return FVector2D(static_cast<float>(ScreenPositionX) / CurrentResolution.X,
	                 static_cast<float>(ScreenPositionY) / CurrentResolution.Y);
}

float UMouseMoveFreeCamera::ConvertAxisMovement(const float NormalizedScreenPosition,
                                                const float MovementThreshold)
{
	const FFloatRange LowerZone = FFloatRange::Inclusive(0, MovementThreshold);
	const FFloatRange DeadZone = FFloatRange::Exclusive(MovementThreshold,
	                                                    (1 - MovementThreshold));
	const FFloatRange UpperZone = FFloatRange::Inclusive(1 - MovementThreshold, 1);

	if (DeadZone.Contains(NormalizedScreenPosition))
	{
		return 0;
	}
	if (LowerZone.Contains(NormalizedScreenPosition))
	{
		const float LowerZoneNormalizedValue = NormalizedScreenPosition /
			MovementThreshold;
		return LowerZoneNormalizedValue - 1;
	}
	if (UpperZone.Contains(NormalizedScreenPosition))
	{
		const float UpperThresholdOffset = NormalizedScreenPosition - UpperZone.
			GetLowerBound().GetValue();
		return UpperThresholdOffset / MovementThreshold;
	}
	checkf(
		true, TEXT(
			"Was unable to map axis screen position to a valid input zone with NormalizedScreenPosition: %f and MovementThreshold: %f."
		), NormalizedScreenPosition, MovementThreshold);
	return 0;
}
