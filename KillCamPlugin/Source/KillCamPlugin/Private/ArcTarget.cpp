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

	ExplosionImpulseStrength = 20000.0f; // Strong enough to scatter
	ExplosionRadius = 150.0f;
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
		// Pass impact direction (inverse of normal)
		Shatter(ImpactPoint, -ImpactNormal);
	}
}

void AArcTarget::Shatter(FVector ImpactPoint, FVector ImpactDirection)
{
	if (bIsShattered) return;

	bIsShattered = true;

	// Trigger Chaos Destruction
	if (GeometryCollection)
	{
		GeometryCollection->SetSimulatePhysics(true);

		// Ensure chunks don't instantly block the arrow that is "sticking" to them
		// Setting to OverlapAll lets visual chunks exist but not block physics immediately
		// Or strictly, we want them to fall but not reject the arrow.
		// However, Chaos often resets collision on fracture.
		// We set to "Destructible" or custom profile, but for now "BlockAll" might be too harsh.
		// Let's ensure the arrow's movement channel is ignored or just rely on physics layers.
		// Use "Destructible" profile which is standard for debris, usually ignoring Pawn/Camera but blocking WorldStatic.
		GeometryCollection->SetCollisionProfileName(TEXT("Destructible"));

		// Apply Radial Impulse if force is configured
		if (ExplosionImpulseStrength > 0.0f)
		{
			FVector Origin = ImpactPoint;
			if (Origin.IsZero()) Origin = GetActorLocation();

			// Apply radial impulse to all chunks
			GeometryCollection->AddRadialImpulse(Origin, ExplosionRadius, ExplosionImpulseStrength, ERadialImpulseFalloff::RIF_Linear, true);
		}
	}

	// Disable detection collision so we can't hit it again
	if (HitCollision)
	{
		HitCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	OnTargetDestroyed.Broadcast(this);
}
