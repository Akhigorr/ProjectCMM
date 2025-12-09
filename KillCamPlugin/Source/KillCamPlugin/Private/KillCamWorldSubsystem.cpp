#include "KillCamWorldSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UKillCamWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ResetEnvironmentStats();
	ActiveKillCamCount = 0;
	TotalScore = 0;
	TimeDilationRequests.Empty();
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

void UKillCamWorldSubsystem::AddScore(int32 Amount)
{
	TotalScore += Amount;
	OnScoreChanged.Broadcast(TotalScore, Amount);
}

void UKillCamWorldSubsystem::ResetScore()
{
	TotalScore = 0;
	OnScoreChanged.Broadcast(TotalScore, 0);
}

void UKillCamWorldSubsystem::RequestTimeDilation(const UObject* Requester, float DilationValue)
{
	if (!Requester) return;
	TimeDilationRequests.Add(Requester, DilationValue);
	UpdateGlobalTimeDilation();
}

void UKillCamWorldSubsystem::ClearTimeDilationRequest(const UObject* Requester)
{
	if (!Requester) return;
	TimeDilationRequests.Remove(Requester);
	UpdateGlobalTimeDilation();
}

void UKillCamWorldSubsystem::UpdateGlobalTimeDilation()
{
	if (!GetWorld()) return;

	float TargetDilation = 1.0f;

	if (TimeDilationRequests.Num() > 0)
	{
		// Filter out stale keys
		TArray<TWeakObjectPtr<const UObject>> StaleKeys;
		float Lowest = 100.0f;

		for (auto It = TimeDilationRequests.CreateIterator(); It; ++It)
		{
			if (It.Key().IsValid())
			{
				if (It.Value() < Lowest)
				{
					Lowest = It.Value();
				}
			}
			else
			{
				StaleKeys.Add(It.Key());
			}
		}

		// Cleanup Stale Keys
		for (const auto& Key : StaleKeys)
		{
			TimeDilationRequests.Remove(Key);
		}

		// Re-evaluate if we just removed everything
		if (TimeDilationRequests.Num() > 0)
		{
			TargetDilation = Lowest;
		}
	}

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TargetDilation);
}

void UKillCamWorldSubsystem::RegisterKillCamStart()
{
	ActiveKillCamCount++;
	if (ActiveKillCamCount == 1)
	{
		EnterKillCamAudioState();
	}
}

void UKillCamWorldSubsystem::RegisterKillCamStop()
{
	ActiveKillCamCount--;
	if (ActiveKillCamCount < 0) ActiveKillCamCount = 0;

	if (ActiveKillCamCount == 0)
	{
		ExitKillCamAudioState();
	}

	// Force an update to clean up any stale keys from destroyed actors
	UpdateGlobalTimeDilation();
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
}

void UKillCamWorldSubsystem::ExitKillCamAudioState()
{
	if (CachedSoundMix && GetWorld())
	{
		UGameplayStatics::PopSoundMixModifier(GetWorld(), CachedSoundMix);
	}
}
