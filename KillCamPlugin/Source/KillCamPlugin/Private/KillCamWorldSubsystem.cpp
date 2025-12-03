#include "KillCamWorldSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UKillCamWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ResetEnvironmentStats();
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

void UKillCamWorldSubsystem::EnterKillCamAudioState()
{
	if (CachedSoundMix && GetWorld())
	{
		UGameplayStatics::PushSoundMixModifier(GetWorld(), CachedSoundMix);
	}

	// If the user provided a sound class, we might want to duck everything else?
	// Usually SoundMix handles the ducking rules internally, so pushing it is enough.
}

void UKillCamWorldSubsystem::ExitKillCamAudioState()
{
	if (CachedSoundMix && GetWorld())
	{
		UGameplayStatics::PopSoundMixModifier(GetWorld(), CachedSoundMix);
	}
}
