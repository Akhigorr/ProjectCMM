#pragma once

#include "CoreMinimal.h"
#include "KillCamTypes.generated.h"

/**
 * Struct to bundle arrow physics settings for easy configuration.
 */
USTRUCT(BlueprintType)
struct FArrowBallisticStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Stats")
	float QuadraticDragCoefficient = 0.0001f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Stats")
	float GravityScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Stats")
	float FletchingRotationSpeed = 360.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Stats")
	float PenetrationDepth = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Stats")
	bool bEnableBounce = true;
};
