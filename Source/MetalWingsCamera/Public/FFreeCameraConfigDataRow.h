#pragma once
#include "Engine/DataTable.h"
#include "FFreeCameraConfigDataRow.generated.h"

USTRUCT(BlueprintType)
struct FFreeCameraConfigDataRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	//

	FFreeCameraConfigDataRow()
	: MoveAcceleration(2000)
	, BrakingAcceleration(2000)
	, MaxSpeed(1000)
	, MinBounds(-10)
	, MaxBounds(10)
	, MouseMovementThreshold(.1f)
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=FreeCamera)
	float MoveAcceleration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=FreeCamera)
	float BrakingAcceleration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=FreeCamera)
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=FreeCamera)
	FVector MinBounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=FreeCamera)
	FVector MaxBounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=FreeCamera)
	FVector2D MouseMovementThreshold;
	
};
