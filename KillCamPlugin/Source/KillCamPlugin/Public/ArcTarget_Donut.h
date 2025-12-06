#pragma once

#include "CoreMinimal.h"
#include "ArcTarget.h"
#include "ArcTarget_Donut.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDonutCenterHit);

/**
 * Has center and outer score zones.
 */
UCLASS()
class KILLCAMPLUGIN_API AArcTarget_Donut : public AArcTarget
{
	GENERATED_BODY()

public:
	AArcTarget_Donut();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arc Target|Donut")
	TObjectPtr<USphereComponent> CenterScoreZone;

	UPROPERTY(BlueprintAssignable, Category = "Arc Target|Donut")
	FOnDonutCenterHit OnCenterHit;

	virtual void HandleHit(FVector ImpactPoint, FVector ImpactNormal, bool bIsRicochet) override;
};
