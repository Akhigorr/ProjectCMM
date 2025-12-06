#pragma once

#include "CoreMinimal.h"
#include "ArcTarget.h"
#include "ArcTarget_BlackHole.h"
#include "ArcTarget_BlackHole.generated.h"

class URadialForceComponent;

/**
 * Pulls arrows in.
 */
UCLASS()
class KILLCAMPLUGIN_API AArcTarget_BlackHole : public AArcTarget
{
	GENERATED_BODY()

public:
	AArcTarget_BlackHole();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arc Target|BlackHole")
	TObjectPtr<URadialForceComponent> PullForce;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arc Target|BlackHole")
	TObjectPtr<USphereComponent> CenterKillZone;

	virtual void HandleHit(FVector ImpactPoint, FVector ImpactNormal, bool bIsRicochet) override;
};
