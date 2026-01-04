#include "KillCamWorldSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UKillCamWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ResetEnvironmentStats();
	ActiveKillCamCount = 0;
}

void UKillCamWorldSubsystem::SetEnvironmentStats(const FArrowEnvironmentStats& NewStats)
{
	CurrentStats = NewStats;
}

void UKillCamWorldSubsystem::ResetEnvironmentStats()
{
	CurrentStats = FArrowEnvironmentStats();
	CurrentStats.GlobalGravityScalar = 1.0f;
	CurrentStats.GlobalDragModifier = 1.0f;
	CurrentStats.GlobalWind = FVector::ZeroVector;
}

void UKillCamWorldSubsystem::SetAudioSettings(USoundMix* Mix, USoundClass* Class)
{
	CachedSoundMix = Mix;
	CachedSoundClass = Class;
}

void UKillCamWorldSubsystem::RegisterKillCamStart(float TargetTimeDilation)
{
	ActiveKillCamCount++;

	if (ActiveKillCamCount == 1)
	{
		// First kill cam active -> Apply Global Effects
		if (GetWorld())
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TargetTimeDilation);

			if (CachedSoundMix)
			{
				UGameplayStatics::PushSoundMixModifier(GetWorld(), CachedSoundMix);
			}
		}
	}
	else
	{
		// Ensure we are using the lowest dilation (slowest) if multiple are active
		float Current = UGameplayStatics::GetGlobalTimeDilation(GetWorld());
		if (TargetTimeDilation < Current)
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TargetTimeDilation);
		}
	}
}

void UKillCamWorldSubsystem::RegisterKillCamStop()
{
	ActiveKillCamCount--;

	if (ActiveKillCamCount <= 0)
	{
		ActiveKillCamCount = 0;

		// Last kill cam ended -> Restore Global Effects
		if (GetWorld())
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);

			if (CachedSoundMix)
			{
				UGameplayStatics::PopSoundMixModifier(GetWorld(), CachedSoundMix);
			}
		}
	}
}
