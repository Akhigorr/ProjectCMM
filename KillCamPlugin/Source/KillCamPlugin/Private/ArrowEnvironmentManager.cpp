#include "ArrowEnvironmentManager.h"
#include "Engine/World.h"

AArrowEnvironmentManager::AArrowEnvironmentManager()
{
	PrimaryActorTick.bCanEverTick = false;

	// Defaults
	EnvironmentStats.GlobalGravityScalar = 1.0f;
	EnvironmentStats.GlobalDragModifier = 1.0f;
}

void AArrowEnvironmentManager::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		UKillCamWorldSubsystem* Subsystem = GetWorld()->GetSubsystem<UKillCamWorldSubsystem>();
		if (Subsystem)
		{
			Subsystem->SetEnvironmentStats(EnvironmentStats);
		}
	}
}
