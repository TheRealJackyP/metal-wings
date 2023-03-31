// Fill out your copyright notice in the Description page of Project Settings.


#include "ViewportUtilities.h"

FVector2D FViewportUtilities::GetGameViewportSize()
{
	FVector2D Result = FVector2D(1, 1);

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(Result);
	}

	return Result;
}
