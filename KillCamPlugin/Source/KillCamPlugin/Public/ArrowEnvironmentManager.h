#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillCamWorldSubsystem.h"
#include "ArrowEnvironmentManager.generated.h"

class USoundMix;
class USoundClass;

/**
 * Place this actor in your level to configure global arrow physics (Wind, Gravity, etc).
 */
UCLASS()
class KILLCAMPLUGIN_API AArrowEnvironmentManager : public AActor
{
	GENERATED_BODY()

public:
	AArrowEnvironmentManager();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/** The environment settings to apply to the World Subsystem on BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Environment")
	FArrowEnvironmentStats EnvironmentStats;

	/** Optional Sound Mix to push when Kill Cam triggers (e.g. muffle ambient noise). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Environment|Audio")
	TObjectPtr<USoundMix> KillCamSoundMix;

	/** Optional Sound Class for specific overrides (optional, depends on SoundMix). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Environment|Audio")
	TObjectPtr<USoundClass> KillCamSoundClass;

};
