#include "KillCamComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Curves/CurveFloat.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "KillCamPlugin.h" // For Logging

UKillCamComponent::UKillCamComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Defaults
	KillCamMode = EKillCamMode::Realtime;
	LookAheadMethod = ELookAheadMethod::Trace;
	TargetTag = "Enemy";
	TargetTimeDilation = 0.2f;
	CameraLagSpeed = 10.0f;
	LookAheadDistance = 5000.0f;
	PredictionRadius = 10.0f;
	bIsKillCamActive = false;

	// Camera Defaults
	bAutoSwitchView = true;
	PostImpactDelay = 2.0f;
	BlendToCamTime = 0.5f;
	BlendBackTime = 0.5f;

	// Debug
	bDebugForceAlwaysTrigger = false;
	bDrawDebugPrediction = false;
}

void UKillCamComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerActor = GetOwner();

	if (KillCamMode == EKillCamMode::Realtime)
	{
		// In Realtime mode, we engage immediately (user can stop it manually if needed)
		// We use a small delay or next tick to ensure everything is initialized
		StartKillCam();
	}
}

void UKillCamComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (bIsKillCamActive)
	{
		StopKillCam();
	}
}

void UKillCamComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsKillCamActive)
	{
		// Auto-Trigger Logic for Predictive Mode
		if (KillCamMode == EKillCamMode::Predictive && OwnerActor.IsValid())
		{
			// Try to trigger prediction every tick
			// Note: For performance, user might want to throttle this, but for "AAA" accuracy, per-tick is needed.
			TriggerLookAhead();
		}
		return;
	}

	if (!OwnerActor.IsValid())
	{
		StopKillCam();
		return;
	}

	// Calculate Speed
	float CurrentSpeed = OwnerActor->GetVelocity().Size();

	// Check for stop/impact to trigger auto-end
	if (bAutoSwitchView && CurrentSpeed < 10.0f)
	{
		// Arrow has stopped.
		UWorld* World = GetWorld();
		if (World && !World->GetTimerManager().IsTimerActive(TimerHandle_StopCam))
		{
			// Start the countdown to return control
			World->GetTimerManager().SetTimer(TimerHandle_StopCam, this, &UKillCamComponent::StopKillCam, PostImpactDelay, false);
		}
	}

	// Logic to update Time Dilation based on curve if present
	if (TimeDilationCurve)
	{
		// Implementation would track time since start and sample curve
	}

	// Prediction for Output
	FHitResult Hit;
	bool bHit = PerformPrediction(Hit);
	float Dist = bHit ? Hit.Distance : -1.0f;

	OnKillCamUpdate.Broadcast(Dist, CurrentSpeed);
}

bool UKillCamComponent::TriggerLookAhead()
{
	if (bDebugForceAlwaysTrigger)
	{
		UE_LOG(LogKillCam, Warning, TEXT("KillCam: Debug Force Triggered for %s"), *GetName());
		StartKillCam();
		return true;
	}

	FHitResult Hit;
	bool bHit = PerformPrediction(Hit);

	if (bHit)
	{
		AActor* HitActor = Hit.GetActor();
		// If we found a target with the tag
		if (HitActor && HitActor->ActorHasTag(TargetTag))
		{
			UE_LOG(LogKillCam, Log, TEXT("KillCam: Target Identified [%s]. Starting Sequence."), *HitActor->GetName());
			StartKillCam();
			return true;
		}
	}
	return false;
}

bool UKillCamComponent::PerformPrediction(FHitResult& OutHit)
{
	if (!OwnerActor.IsValid()) return false;

	FVector Start = OwnerActor->GetActorLocation();
	FVector Velocity = OwnerActor->GetVelocity();
	FVector Direction = Velocity.GetSafeNormal();

	// Fallback if velocity is zero (just spawned)
	if (Direction.IsZero())
	{
		Direction = OwnerActor->GetActorForwardVector();
	}

	if (LookAheadMethod == ELookAheadMethod::Trace)
	{
		FVector End = Start + (Direction * LookAheadDistance);

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(OwnerActor.Get());

		EDrawDebugTrace::Type DebugType = bDrawDebugPrediction ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		return UKismetSystemLibrary::SphereTraceSingle(
			this,
			Start,
			End,
			PredictionRadius,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			ActorsToIgnore,
			DebugType,
			OutHit,
			true
		);
	}
	else if (LookAheadMethod == ELookAheadMethod::Physics)
	{
		FPredictProjectilePathParams PathParams;
		PathParams.StartLocation = Start;
		PathParams.LaunchVelocity = Velocity;
		PathParams.bTraceWithCollision = true;
		PathParams.ProjectileRadius = PredictionRadius;
		PathParams.MaxSimTime = 2.0f; // Simulate up to 2 seconds ahead
		PathParams.bTraceWithChannel = true;
		PathParams.TraceChannel = ECC_Visibility;
		PathParams.ActorsToIgnore.Add(OwnerActor.Get());
		PathParams.DrawDebugType = bDrawDebugPrediction ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
		PathParams.DrawDebugTime = 1.0f;

		// Typically we might want to check against specific object types, but channel is generic enough

		FPredictProjectilePathResult PathResult;
		bool bSuccess = UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);

		if (PathResult.HitResult.bBlockingHit)
		{
			OutHit = PathResult.HitResult;
			return true;
		}
	}

	return false;
}

void UKillCamComponent::StartKillCam()
{
	if (bIsKillCamActive) return;

	UE_LOG(LogKillCam, Log, TEXT("KillCam: Sequence STARTED. Dilation: %f"), TargetTimeDilation);

	bIsKillCamActive = true;
	UGameplayStatics::SetGlobalTimeDilation(this, TargetTimeDilation);

	if (bAutoSwitchView && OwnerActor.IsValid())
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC)
		{
			// Cache original
			OriginalViewTarget = PC->GetViewTarget();
			// Switch
			PC->SetViewTargetWithBlend(OwnerActor.Get(), BlendToCamTime, VTBlend_EaseInOut, 1.0f);
		}
	}

	// Audio Start
	if (GetWorld())
	{
		if (UKillCamWorldSubsystem* Subsystem = GetWorld()->GetSubsystem<UKillCamWorldSubsystem>())
		{
			Subsystem->EnterKillCamAudioState();
		}
	}

	OnKillCamStart.Broadcast();
}

void UKillCamComponent::StopKillCam()
{
	if (!bIsKillCamActive) return;

	UE_LOG(LogKillCam, Log, TEXT("KillCam: Sequence ENDED. Restoring state."));

	bIsKillCamActive = false;
	UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);

	// Audio Stop
	if (GetWorld())
	{
		if (UKillCamWorldSubsystem* Subsystem = GetWorld()->GetSubsystem<UKillCamWorldSubsystem>())
		{
			Subsystem->ExitKillCamAudioState();
		}
	}

	if (bAutoSwitchView)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC && OriginalViewTarget.IsValid())
		{
			// Return control
			PC->SetViewTargetWithBlend(OriginalViewTarget.Get(), BlendBackTime, VTBlend_EaseInOut, 1.0f);
		}
		else if (PC && PC->GetPawn())
		{
			// Fallback to pawn
			PC->SetViewTargetWithBlend(PC->GetPawn(), BlendBackTime, VTBlend_EaseInOut, 1.0f);
		}
	}

	OnKillCamEnd.Broadcast();
}
