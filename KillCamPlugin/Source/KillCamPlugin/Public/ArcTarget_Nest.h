#pragma once

#include "CoreMinimal.h"
#include "ArcTarget.h"
#include "ArcTarget_Nest.generated.h"

/**
 * Respawns child target.
 */
UCLASS()
class KILLCAMPLUGIN_API AArcTarget_Nest : public AArcTarget
{
	GENERATED_BODY()

public:
	AArcTarget_Nest();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target|Nest")
	float RegenTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target|Nest")
	TObjectPtr<AArcTarget> ChildTarget;

	virtual void BeginPlay() override;

private:
	FTimerHandle TimerHandle_Respawn;

	UFUNCTION()
	void OnChildDestroyed(AArcTarget* DestroyedTarget);

	void RespawnChild();
};
