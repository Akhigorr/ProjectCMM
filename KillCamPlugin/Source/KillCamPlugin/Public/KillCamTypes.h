#pragma once

#include "CoreMinimal.h"
#include "KillCamTypes.generated.h"

UENUM(BlueprintType)
enum class EArrowPreset : uint8
{
	Standard		UMETA(DisplayName = "Standard Arrow"),
	Heavy			UMETA(DisplayName = "Heavy Bolt"),
	Sniper			UMETA(DisplayName = "Sniper / Fast"),
	Arcade			UMETA(DisplayName = "Arcade / No Drag"),
	Floater			UMETA(DisplayName = "Low Gravity / Floater")
};

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
