#pragma once

#include "CoreMinimal.h"
#include "ArcTarget.h"
#include "ArcTarget_SplineMover.generated.h"

class USplineComponent;

/**
 * Moves back and forth along a Spline Component.
 */
UCLASS()
class KILLCAMPLUGIN_API AArcTarget_SplineMover : public AArcTarget
{
	GENERATED_BODY()

public:
	AArcTarget_SplineMover();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target|Spline")
	TObjectPtr<AActor> SplinePathActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target|Spline")
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target|Spline")
	bool bPingPong;

private:
	float DistanceAlongSpline;
	int Direction;
};
