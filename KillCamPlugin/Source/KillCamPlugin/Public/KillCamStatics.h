#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KillCamTypes.h"
#include "KillCamStatics.generated.h"

/**
 * Helper library for Kill Cam physics and calculations.
 */
UCLASS()
class KILLCAMPLUGIN_API UKillCamStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Returns pre-configured ballistic stats for common arrow types.
	 */
	UFUNCTION(BlueprintPure, Category = "Kill Cam|Physics")
	static FArrowBallisticStats GetArrowPreset(EArrowPreset Preset);

	/**
	 * Predicts the path of an arrow using the plugin's specific physics model (Drag, Wind, Global Gravity).
	 * Useful for drawing aiming lines/reticles without spawning a real arrow.
	 */
	UFUNCTION(BlueprintCallable, Category = "Kill Cam|Physics", meta = (WorldContext = "WorldContextObject"))
	static bool PredictArrowPath(const UObject* WorldContextObject, FVector StartPos, FVector LaunchVelocity,
		float GravityScale, float DragCoefficient, FVector WindVector,
		float SimTime, float SimStep, float Radius,
		TArray<FVector>& OutPathPositions, FHitResult& OutHit);

	/**
	 * Calculates the launch velocity required to hit a target location.
	 * Note: Solves for a standard arc. Returns false if out of range or impossible.
	 * Simple ballistic approximation (ignores drag/wind for the calculation, so aims slightly high).
	 */
	UFUNCTION(BlueprintPure, Category = "Kill Cam|Physics", meta = (WorldContext = "WorldContextObject"))
	static bool SuggestTossVelocity(const UObject* WorldContextObject, FVector Start, FVector End, float TossSpeed, float GravityZ, FVector& OutVelocity, bool bHighArc = false);

};
