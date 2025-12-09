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
			Subsystem->SetAudioSettings(KillCamSoundMix, KillCamSoundClass);
		}
	}
}

void AArrowEnvironmentManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Reset stats when this manager is removed (e.g. streaming unload)
	if (GetWorld())
	{
		UKillCamWorldSubsystem* Subsystem = GetWorld()->GetSubsystem<UKillCamWorldSubsystem>();
		if (Subsystem)
		{
			Subsystem->ResetEnvironmentStats();
		}
	}
}
