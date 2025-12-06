#include "ArcTarget.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "KillCamPlugin.h"

AArcTarget::AArcTarget()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	GeometryCollection->SetupAttachment(RootComponent);
	GeometryCollection->SetSimulatePhysics(false);
	GeometryCollection->SetCollisionProfileName("BlockAll");

	HitCollision = CreateDefaultSubobject<USphereComponent>(TEXT("HitCollision"));
	HitCollision->SetupAttachment(GeometryCollection);
	HitCollision->InitSphereRadius(50.0f);
	HitCollision->SetCollisionProfileName("OverlapAllDynamic");

	Health = 100.0f;
	ScoreValue = 10;
	bIsShattered = false;
}

void AArcTarget::BeginPlay()
{
	Super::BeginPlay();
}

void AArcTarget::HandleHit(FVector ImpactPoint, FVector ImpactNormal, bool bIsRicochet)
{
	if (bIsShattered) return;

	Health -= 100.0f;

	if (Health <= 0.0f)
	{
		Shatter();
	}
}

void AArcTarget::Shatter()
{
	if (bIsShattered) return;

	bIsShattered = true;

	// Trigger Chaos Destruction
	if (GeometryCollection)
	{
		GeometryCollection->SetSimulatePhysics(true);
	}

	// Disable detection collision so we can't hit it again
	if (HitCollision)
	{
		HitCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	OnTargetDestroyed.Broadcast(this);
}
