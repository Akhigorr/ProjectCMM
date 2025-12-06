#include "ArcTarget_Mirror.h"

void AArcTarget_Mirror::HandleHit(FVector ImpactPoint, FVector ImpactNormal, bool bIsRicochet)
{
	if (bIsRicochet)
	{
		Super::HandleHit(ImpactPoint, ImpactNormal, bIsRicochet);
	}
	else
	{
		// Reflect logic is assumed to be handled by the Arrow's physics material if it doesn't pierce.
		// But here we explicitly deny the shatter.
		UE_LOG(LogTemp, Log, TEXT("Mirror Target hit directly - Deflected!"));
	}
}
