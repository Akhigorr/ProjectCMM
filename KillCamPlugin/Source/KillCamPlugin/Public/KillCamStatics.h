#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KillCamTypes.h"
#include "KillCamStatics.generated.h"

class UKillCamWorldSubsystem;

/**
 * Static helper library for shared Kill Cam math (Prediction, Physics).
 */
UCLASS()
class KILLCAMPLUGIN_API UKillCamStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Calculates the instantaneous acceleration for an arrow based on physics forces.
	 * Accounts for Gravity (Local + Global), Drag (Quadratic), and Wind.
	 */
	UFUNCTION(BlueprintPure, Category = "Kill Cam|Physics")
	static FVector GetArrowAcceleration(const FVector& Velocity, float LocalGravityZ, const FArrowBallisticStats& Stats, const UKillCamWorldSubsystem* Subsystem);

	/**
	 * Predicts the path of an arrow using the custom physics model (Drag, Wind, Variable Gravity).
	 * Returns true if a blocking hit was found.
	 */
	UFUNCTION(BlueprintCallable, Category = "Kill Cam|Physics", meta = (WorldContext = "WorldContextObject"))
	static bool PredictArrowPath(const UObject* WorldContextObject, const FVector& StartLocation, const FVector& StartVelocity,
		const FArrowBallisticStats& Stats, float MaxSimTime, float StepFrequency, float Radius,
		const TArray<AActor*>& ActorsToIgnore, FHitResult& OutHit, TArray<FVector>& OutPathPoints);

	/**
	 * Helper to get the subsystem safely.
	 */
	static UKillCamWorldSubsystem* GetKillCamSubsystem(const UObject* WorldContextObject);
};
