#include "ArcTarget.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
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

float AArcTarget::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (bIsShattered) return 0.0f;

	// Extract Hit Info if available
	FVector ImpactPoint = GetActorLocation();
	FVector ImpactNormal = FVector::UpVector;

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		ImpactPoint = PointDamageEvent->HitInfo.ImpactPoint;
		ImpactNormal = PointDamageEvent->HitInfo.ImpactNormal;
	}

	// For now, assume generic hits are not ricochets if they deal damage
	HandleHit(ImpactPoint, ImpactNormal, false);

	return ActualDamage;
}

void AArcTarget::HandleHit(FVector ImpactPoint, FVector ImpactNormal, bool bIsRicochet)
{
	if (bIsShattered) return;

	Health -= 100.0f; // Assuming 1 hit kill or using DamageAmount passed via HandleHit?
	// Original code hardcoded -100.0f. We keep it consistent.
	// Improvements could involve passing damage amount to HandleHit.

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

		// Change collision profile to prevent debris blocking new arrows
		GeometryCollection->SetCollisionProfileName("Destructible");

		// Apply impulse to make it look like an explosion/shatter
		// Default values, can be exposed if needed
		float ImpulseStrength = 500.0f;
		float ImpulseRadius = 100.0f;

		FVector Center = GetActorLocation();

		GeometryCollection->AddRadialImpulse(Center, ImpulseRadius, ImpulseStrength, ERadialImpulseFalloff::RIF_Linear, true);
	}

	// Disable detection collision so we can't hit it again
	if (HitCollision)
	{
		HitCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	OnTargetDestroyed.Broadcast(this);
}
