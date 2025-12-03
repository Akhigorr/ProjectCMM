#include "KillCamWorldSubsystem.h"

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
