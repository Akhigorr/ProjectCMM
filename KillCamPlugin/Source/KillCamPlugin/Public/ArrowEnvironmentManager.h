#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillCamWorldSubsystem.h"
#include "ArrowEnvironmentManager.generated.h"

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

public:
	/** The environment settings to apply to the World Subsystem on BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Environment")
	FArrowEnvironmentStats EnvironmentStats;

};
