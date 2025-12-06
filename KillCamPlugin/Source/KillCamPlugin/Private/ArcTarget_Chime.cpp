#include "ArcTarget_Chime.h"

void AArcTarget_Chime::HandleHit(FVector ImpactPoint, FVector ImpactNormal, bool bIsRicochet)
{
	OnChimeHit.Broadcast(SequenceID);
	Super::HandleHit(ImpactPoint, ImpactNormal, bIsRicochet);
}
