#pragma once

#include "CoreMinimal.h"
#include "ArcTarget.h"
#include "ArcTarget_Shielded.generated.h"

/**
 * Only shatters from behind.
 */
UCLASS()
class KILLCAMPLUGIN_API AArcTarget_Shielded : public AArcTarget
{
	GENERATED_BODY()

public:
	virtual void HandleHit(FVector ImpactPoint, FVector ImpactNormal, bool bIsRicochet) override;
};
