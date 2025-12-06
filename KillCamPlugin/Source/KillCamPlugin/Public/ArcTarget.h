#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArcTarget.generated.h"

class UGeometryCollectionComponent;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArcTargetDestroyed, AArcTarget*, DestroyedTarget);

UCLASS(Abstract)
class KILLCAMPLUGIN_API AArcTarget : public AActor
{
	GENERATED_BODY()

public:
	AArcTarget();

protected:
	virtual void BeginPlay() override;

public:
	// --- Components ---

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arc Target")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arc Target")
	TObjectPtr<UGeometryCollectionComponent> GeometryCollection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arc Target")
	TObjectPtr<USphereComponent> HitCollision;

	// --- Variables ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target|Stats")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target|Stats")
	int32 ScoreValue;

	UPROPERTY(BlueprintReadOnly, Category = "Arc Target|State")
	bool bIsShattered;

	UPROPERTY(BlueprintAssignable, Category = "Arc Target|Events")
	FOnArcTargetDestroyed OnTargetDestroyed;

	// --- Functions ---

	UFUNCTION(BlueprintCallable, Category = "Arc Target")
	virtual void HandleHit(FVector ImpactPoint, FVector ImpactNormal, bool bIsRicochet);

	UFUNCTION(BlueprintCallable, Category = "Arc Target")
	virtual void Shatter();

};
