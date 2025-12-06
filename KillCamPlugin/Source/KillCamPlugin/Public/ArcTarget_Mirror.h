#pragma once

#include "CoreMinimal.h"
#include "ArcTarget.h"
#include "ArcTarget_Mirror.generated.h"

/**
 * Reflects normal shots, shatters on ricochets.
 */
UCLASS()
class KILLCAMPLUGIN_API AArcTarget_Mirror : public AArcTarget
{
	GENERATED_BODY()

public:
	virtual void HandleHit(FVector ImpactPoint, FVector ImpactNormal, bool bIsRicochet) override;
};
