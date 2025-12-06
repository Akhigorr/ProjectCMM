#pragma once

#include "CoreMinimal.h"
#include "ArcTarget.h"
#include "ArcTarget_Orbiter.generated.h"

/**
 * Rotates around a pivot actor.
 */
UCLASS()
class KILLCAMPLUGIN_API AArcTarget_Orbiter : public AArcTarget
{
	GENERATED_BODY()

public:
	AArcTarget_Orbiter();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target|Orbiter")
	TObjectPtr<AActor> PivotActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target|Orbiter")
	float RotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target|Orbiter")
	float Radius;

private:
	float CurrentAngle;
};
