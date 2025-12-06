#include "ArcTarget_Donut.h"
#include "Components/SphereComponent.h"

AArcTarget_Donut::AArcTarget_Donut()
{
	CenterScoreZone = CreateDefaultSubobject<USphereComponent>(TEXT("CenterScoreZone"));
	CenterScoreZone->SetupAttachment(RootComponent);
	CenterScoreZone->InitSphereRadius(15.0f);
}

void AArcTarget_Donut::HandleHit(FVector ImpactPoint, FVector ImpactNormal, bool bIsRicochet)
{
	// Check distance to center to distinguish zones
	float DistSq = FVector::DistSquared(ImpactPoint, GetActorLocation());

	if (DistSq < FMath::Square(CenterScoreZone->GetScaledSphereRadius()))
	{
		OnCenterHit.Broadcast();
		ScoreValue *= 2; // Bonus logic example
	}

	Super::HandleHit(ImpactPoint, ImpactNormal, bIsRicochet);
}
