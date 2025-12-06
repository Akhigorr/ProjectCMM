#include "ArcTarget_BlackHole.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/SphereComponent.h"

AArcTarget_BlackHole::AArcTarget_BlackHole()
{
	PullForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("PullForce"));
	PullForce->SetupAttachment(RootComponent);
	PullForce->Radius = 1000.0f;
	PullForce->ForceStrength = -200000.0f; // Negative pull
	PullForce->ImpulseStrength = -1000.0f;

	CenterKillZone = CreateDefaultSubobject<USphereComponent>(TEXT("CenterKillZone"));
	CenterKillZone->SetupAttachment(RootComponent);
	CenterKillZone->InitSphereRadius(20.0f);
}

void AArcTarget_BlackHole::HandleHit(FVector ImpactPoint, FVector ImpactNormal, bool bIsRicochet)
{
	// Only shatter if hit in the center
	float DistSq = FVector::DistSquared(ImpactPoint, GetActorLocation());
	if (DistSq < FMath::Square(CenterKillZone->GetScaledSphereRadius()))
	{
		Super::HandleHit(ImpactPoint, ImpactNormal, bIsRicochet);
	}
}
