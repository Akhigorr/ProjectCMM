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

	// --- Audio Management ---

	/** Sets the sound mix to use during Kill Cam (e.g., to mute background noise). */
	UFUNCTION(BlueprintCallable, Category = "Arrow Environment|Audio")
	void SetAudioSettings(USoundMix* Mix, USoundClass* Class);

	/** Activates the specialized audio mix for slow motion. */
	void EnterKillCamAudioState();

	/** Clears the specialized audio mix. */
	void ExitKillCamAudioState();

private:
	FArrowEnvironmentStats CurrentStats;

	TObjectPtr<USoundMix> CachedSoundMix;
	TObjectPtr<USoundClass> CachedSoundClass;
};
