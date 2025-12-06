#include "ArcTarget_Shielded.h"
#include "KillCamPlugin.h"

void AArcTarget_Shielded::HandleHit(FVector ImpactPoint, FVector ImpactNormal, bool bIsRicochet)
{
	// ImpactNormal usually points OUT of the surface (towards the projectile).
	// Forward Vector points forward.
	// Hitting the front: Normal opposes Forward (Dot < 0).
	// Hitting the back: Normal aligns with Forward (Dot > 0).

	float Dot = FVector::DotProduct(ImpactNormal, GetActorForwardVector());

	if (Dot > 0.0f)
	{
		Super::HandleHit(ImpactPoint, ImpactNormal, bIsRicochet);
	}
	else
	{
		// Clank logic (Play Sound)
		UE_LOG(LogKillCam, Log, TEXT("Shielded Target hit from front - Clank!"));
	}
}
