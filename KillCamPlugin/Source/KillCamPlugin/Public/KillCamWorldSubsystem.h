#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "KillCamWorldSubsystem.generated.h"

/**
 * Struct defining global environmental factors for arrows in this world.
 */
USTRUCT(BlueprintType)
struct FArrowEnvironmentStats
{
	GENERATED_BODY()

	/** Global wind force applied to all arrows. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Environment")
	FVector GlobalWind = FVector::ZeroVector;

	/** Multiplier for arrow gravity (e.g., 0.5 for low gravity level). Default 1.0. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Environment")
	float GlobalGravityScalar = 1.0f;

	/** Multiplier for air resistance (e.g., 2.0 for thick fog/water). Default 1.0. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Environment")
	float GlobalDragModifier = 1.0f;
};

/**
 * Manages global arrow physics settings for the current world/level.
 */
UCLASS()
class KILLCAMPLUGIN_API UKillCamWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Sets the global environment stats for this level. */
	UFUNCTION(BlueprintCallable, Category = "Arrow Environment")
	void SetEnvironmentStats(const FArrowEnvironmentStats& NewStats);

	/** Resets to defaults. */
	UFUNCTION(BlueprintCallable, Category = "Arrow Environment")
	void ResetEnvironmentStats();

	// --- Getters ---

	UFUNCTION(BlueprintPure, Category = "Arrow Environment")
	FVector GetGlobalWind() const { return CurrentStats.GlobalWind; }

	UFUNCTION(BlueprintPure, Category = "Arrow Environment")
	float GetGlobalGravityScalar() const { return CurrentStats.GlobalGravityScalar; }

	UFUNCTION(BlueprintPure, Category = "Arrow Environment")
	float GetGlobalDragModifier() const { return CurrentStats.GlobalDragModifier; }

	// --- Time Dilation Management ---

	/** Requests a specific time dilation value. Lowest value wins across all requests. */
	UFUNCTION(BlueprintCallable, Category = "Kill Cam|Time")
	void RequestTimeDilation(FName Reason, float DilationValue);

	/** Removes a time dilation request. */
	UFUNCTION(BlueprintCallable, Category = "Kill Cam|Time")
	void ClearTimeDilationRequest(FName Reason);

	/** Gets the current active kill cam count. */
	UFUNCTION(BlueprintPure, Category = "Kill Cam")
	int32 GetActiveKillCamCount() const { return ActiveKillCamCount; }

	/** Register that a Kill Cam sequence has started. */
	void RegisterKillCamStart();

	/** Register that a Kill Cam sequence has ended. */
	void RegisterKillCamStop();

	// --- Audio Management ---

	/** Sets the sound mix to use during Kill Cam (e.g., to mute background noise). */
	UFUNCTION(BlueprintCallable, Category = "Arrow Environment|Audio")
	void SetAudioSettings(USoundMix* Mix, USoundClass* Class);

private:
	FArrowEnvironmentStats CurrentStats;

	TObjectPtr<USoundMix> CachedSoundMix;
	TObjectPtr<USoundClass> CachedSoundClass;

	// Time Dilation
	TMap<FName, float> TimeDilationRequests;
	void UpdateGlobalTimeDilation();

	// Kill Cam Tracking
	int32 ActiveKillCamCount = 0;
	void EnterKillCamAudioState();
	void ExitKillCamAudioState();
};
