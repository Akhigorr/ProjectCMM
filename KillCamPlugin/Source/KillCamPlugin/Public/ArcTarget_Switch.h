#pragma once

#include "CoreMinimal.h"
#include "ArcTarget.h"
#include "ArcTarget_Switch.generated.h"

/**
 * Hides/Unhides another target on hit.
 */
UCLASS()
class KILLCAMPLUGIN_API AArcTarget_Switch : public AArcTarget
{
	GENERATED_BODY()

public:
	AArcTarget_Switch();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target|Switch")
	TObjectPtr<AArcTarget> LinkedTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target|Switch")
	float RevealDuration;

	virtual void HandleHit(FVector ImpactPoint, FVector ImpactNormal, bool bIsRicochet) override;

private:
	FTimerHandle TimerHandle_HideLinked;
	void HideLinkedTarget();
};
