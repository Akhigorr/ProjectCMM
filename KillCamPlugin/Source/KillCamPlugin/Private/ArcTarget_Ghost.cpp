#include "ArcTarget_Ghost.h"
#include "Components/SphereComponent.h"

AArcTarget_Ghost::AArcTarget_Ghost()
{
	bIsVisibleToLens = false;
	// Start hidden/no collision
	if (HitCollision)
	{
		HitCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	SetActorHiddenInGame(true);
}

void AArcTarget_Ghost::SetGhostVisibility_Implementation(bool bVisible)
{
	bIsVisibleToLens = bVisible;
	SetActorHiddenInGame(!bVisible);

	if (HitCollision)
	{
		HitCollision->SetCollisionEnabled(bVisible ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	}
}
