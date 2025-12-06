#include "ArcTarget_Switch.h"
#include "TimerManager.h"

AArcTarget_Switch::AArcTarget_Switch()
{
	RevealDuration = 5.0f;
}

void AArcTarget_Switch::HandleHit(FVector ImpactPoint, FVector ImpactNormal, bool bIsRicochet)
{
	// Do not shatter.

	if (LinkedTarget)
	{
		LinkedTarget->SetActorHiddenInGame(false);
		LinkedTarget->SetActorEnableCollision(true); // Assuming we want to enable it too

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_HideLinked, this, &AArcTarget_Switch::HideLinkedTarget, RevealDuration, false);
	}
}

void AArcTarget_Switch::HideLinkedTarget()
{
	if (LinkedTarget)
	{
		LinkedTarget->SetActorHiddenInGame(true);
		LinkedTarget->SetActorEnableCollision(false);
	}
}
