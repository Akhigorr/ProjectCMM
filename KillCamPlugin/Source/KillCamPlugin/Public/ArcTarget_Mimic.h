#pragma once

#include "CoreMinimal.h"
#include "ArcTarget.h"
#include "ArcTarget_Mimic.generated.h"

/**
 * Dodges when a projectile enters its detection radius.
 */
UCLASS()
class KILLCAMPLUGIN_API AArcTarget_Mimic : public AArcTarget
{
	GENERATED_BODY()

public:
	AArcTarget_Mimic();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arc Target|Mimic")
	TObjectPtr<USphereComponent> DetectionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target|Mimic")
	float DodgeSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target|Mimic")
	float DodgeDistance;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	FVector TargetDodgeLocation;
	bool bIsDodging;
};
