#include "KillCamComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Curves/CurveFloat.h"
#include "Engine/World.h"

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
}

void UKillCamComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerActor = GetOwner();
}

void UKillCamComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsKillCamActive)
	{
		// If in realtime mode, we might want to trigger automatically if we are just flying?
		// For now, "Realtime" assumes the camera is already there, but "Active" means the *Effect* (slowmo/zoom) is on.
		return;
	}

	if (!OwnerActor.IsValid())
	{
		StopKillCam();
		return;
	}

	// Logic to update Time Dilation based on curve if present
	if (TimeDilationCurve)
	{
		// Implementation would track time since start and sample curve
		// For simplicity in this iteration, we stick to fixed TargetTimeDilation or could add a timer.
	}

	// Calculate distance to potential target if we have a locked target,
	// or just arbitrary distance for the sake of the output.
	// For this generic component, let's output speed ratio.
	float CurrentSpeed = OwnerActor->GetVelocity().Size();

	// We broadcast generic values for the user to hook into PostProcess
	// We could Raycast here to get "DistanceToTarget" every frame if we wanted precise focus.
	FHitResult Hit;
	bool bHit = PerformPrediction(Hit);
	float Dist = bHit ? Hit.Distance : -1.0f;

	OnKillCamUpdate.Broadcast(Dist, CurrentSpeed);
}

bool UKillCamComponent::TriggerLookAhead()
{
	FHitResult Hit;
	bool bHit = PerformPrediction(Hit);

	if (bHit)
	{
		// If we found a target with the tag
		if (Hit.GetActor() && Hit.GetActor()->ActorHasTag(TargetTag))
		{
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

		return UKismetSystemLibrary::SphereTraceSingle(
			this,
			Start,
			End,
			PredictionRadius,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
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

	bIsKillCamActive = true;
	UGameplayStatics::SetGlobalTimeDilation(this, TargetTimeDilation);
	OnKillCamStart.Broadcast();
}

void UKillCamComponent::StopKillCam()
{
	if (!bIsKillCamActive) return;

	bIsKillCamActive = false;
	UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
	OnKillCamEnd.Broadcast();
}
