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

void UKillCamWorldSubsystem::RequestTimeDilation(FName Reason, float DilationValue)
{
	TimeDilationRequests.FindOrAdd(Reason) = DilationValue;
	UpdateGlobalTimeDilation();
}

void UKillCamWorldSubsystem::ClearTimeDilationRequest(FName Reason)
{
	TimeDilationRequests.Remove(Reason);
	UpdateGlobalTimeDilation();
}

void UKillCamWorldSubsystem::UpdateGlobalTimeDilation()
{
	if (!GetWorld()) return;

	float TargetDilation = 1.0f;

	if (TimeDilationRequests.Num() > 0)
	{
		// Find lowest value
		float Lowest = 100.0f; // Arbitrary high start
		for (const auto& Pair : TimeDilationRequests)
		{
			if (Pair.Value < Lowest)
			{
				Lowest = Pair.Value;
			}
		}
		TargetDilation = Lowest;
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
