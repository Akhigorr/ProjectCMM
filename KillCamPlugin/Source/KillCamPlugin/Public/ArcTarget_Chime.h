#pragma once

#include "CoreMinimal.h"
#include "ArcTarget.h"
#include "ArcTarget_Chime.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChimeHit, int32, SequenceID);

/**
 * Broadcasts ID for musical puzzles.
 */
UCLASS()
class KILLCAMPLUGIN_API AArcTarget_Chime : public AArcTarget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target|Chime")
	int32 SequenceID;

	UPROPERTY(BlueprintAssignable, Category = "Arc Target|Chime")
	FOnChimeHit OnChimeHit;

	virtual void HandleHit(FVector ImpactPoint, FVector ImpactNormal, bool bIsRicochet) override;
};
