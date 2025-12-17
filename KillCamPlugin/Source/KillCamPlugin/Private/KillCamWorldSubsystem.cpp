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

void UKillCamWorldSubsystem::RegisterTimeDilationRequest(const UObject* Requester, float Value)
{
	if (!Requester) return;

	TimeDilationRequests.Add(Requester, Value);
	UpdateGlobalTimeDilation();
}

void UKillCamWorldSubsystem::UnregisterTimeDilationRequest(const UObject* Requester)
{
	if (!Requester) return;

	TimeDilationRequests.Remove(Requester);
	UpdateGlobalTimeDilation();
}

void UKillCamWorldSubsystem::UpdateGlobalTimeDilation()
{
	if (!GetWorld()) return;

	float LowestDilation = 1.0f;
	bool bHasActiveRequests = false;

	// Iterate and find the lowest value
	for (auto It = TimeDilationRequests.CreateIterator(); It; ++It)
	{
		// Check validity of the requester (WeakPtr)
		if (It.Key().IsValid())
		{
			float Val = It.Value();
			if (Val < LowestDilation)
			{
				LowestDilation = Val;
			}
			bHasActiveRequests = true;
		}
		else
		{
			// Cleanup invalid/dead objects
			It.RemoveCurrent();
		}
	}

	// Apply
	UGameplayStatics::SetGlobalTimeDilation(this, LowestDilation);
}
