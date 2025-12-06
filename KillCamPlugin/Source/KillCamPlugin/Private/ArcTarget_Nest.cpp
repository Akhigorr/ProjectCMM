#include "ArcTarget_Nest.h"
#include "TimerManager.h"

AArcTarget_Nest::AArcTarget_Nest()
{
	RegenTime = 10.0f;
}

void AArcTarget_Nest::BeginPlay()
{
	Super::BeginPlay();

	if (ChildTarget)
	{
		ChildTarget->OnTargetDestroyed.AddDynamic(this, &AArcTarget_Nest::OnChildDestroyed);
	}
}

void AArcTarget_Nest::OnChildDestroyed(AArcTarget* DestroyedTarget)
{
	if (DestroyedTarget == ChildTarget)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Respawn, this, &AArcTarget_Nest::RespawnChild, RegenTime, false);
	}
}

void AArcTarget_Nest::RespawnChild()
{
	if (ChildTarget)
	{
		// Reset logic depends on ChildTarget implementation (e.g. Health = 100, bIsShattered = false, Reset GeometryCollection)
		// For base class, we just unhide and reset variables manually for now.
		ChildTarget->bIsShattered = false;
		ChildTarget->Health = 100.0f;
		// Note: Geometry Collections are hard to "reset" without respawning actor.
		// Ideally we spawn a new actor class here.
		// For prototype, we assume we can just unhide.
	}
}
