#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArcTargetComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTargetShattered);

/**
 * Component to turn any Actor into a Kill Cam Target.
 * Handles Health, Scoring, and triggering Chaos destruction.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KILLCAMPLUGIN_API UArcTargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UArcTargetComponent();

protected:
	virtual void BeginPlay() override;

public:
	// --- Config ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target")
	int32 ScoreValue;

	/** If true, automatically finds a GeometryCollectionComponent on the owner to shatter. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target")
	bool bAutoFindGeometryCollection;

	// --- Events ---

	UPROPERTY(BlueprintAssignable, Category = "Arc Target")
	FOnTargetShattered OnShattered;

	// --- Functions ---

	UFUNCTION(BlueprintCallable, Category = "Arc Target")
	void TakeDamage(float Amount, FVector ImpactPoint);

	UFUNCTION(BlueprintCallable, Category = "Arc Target")
	void Shatter(FVector ImpactPoint);

private:
	bool bIsShattered;

	UFUNCTION()
	void OnOwnerTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnOwnerTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);
};
