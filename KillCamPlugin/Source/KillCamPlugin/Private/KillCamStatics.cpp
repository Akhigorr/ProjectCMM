#include "KillCamStatics.h"
#include "KillCamWorldSubsystem.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"

FVector UKillCamStatics::GetArrowAcceleration(const FVector& Velocity, float LocalGravityZ, const FArrowBallisticStats& Stats, const UKillCamWorldSubsystem* Subsystem)
{
	float FinalGravityScale = Stats.GravityScale;
	float FinalDrag = Stats.QuadraticDragCoefficient;
	FVector FinalWind = FVector::ZeroVector;

	if (Subsystem)
	{
		FinalGravityScale *= Subsystem->GetGlobalGravityScalar();
		FinalDrag *= Subsystem->GetGlobalDragModifier();
		FinalWind = Subsystem->GetGlobalWind();
	}

	FVector Acceleration = FVector::ZeroVector;

	// 1. Gravity (Z-axis)
	Acceleration.Z += LocalGravityZ * FinalGravityScale;

	// 2. Drag ( -C * v^2 * direction )
	if (FinalDrag > 0.0f && !Velocity.IsZero())
	{
		float SpeedSq = Velocity.SizeSquared();
		FVector DragForce = -Velocity.GetSafeNormal() * (FinalDrag * SpeedSq);
		Acceleration += DragForce;
	}

	// 3. Wind
	Acceleration += FinalWind;

	return Acceleration;
}

UKillCamWorldSubsystem* UKillCamStatics::GetKillCamSubsystem(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;
	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return nullptr;
	return World->GetSubsystem<UKillCamWorldSubsystem>();
}

bool UKillCamStatics::PredictArrowPath(const UObject* WorldContextObject, const FVector& StartLocation, const FVector& StartVelocity,
	const FArrowBallisticStats& Stats, float MaxSimTime, float StepFrequency, float Radius,
	const TArray<AActor*>& ActorsToIgnore, FHitResult& OutHit, TArray<FVector>& OutPathPoints)
{
	if (!WorldContextObject) return false;

	UKillCamWorldSubsystem* Subsystem = GetKillCamSubsystem(WorldContextObject);
	UWorld* World = WorldContextObject->GetWorld();
	float GravityZ = World ? World->GetGravityZ() : -980.0f;

	float SimTime = 0.0f;
	float StepTime = (StepFrequency > 0.0f) ? (1.0f / StepFrequency) : 0.033f; // Default 30hz

	FVector CurrentPos = StartLocation;
	FVector CurrentVel = StartVelocity;
	OutPathPoints.Add(CurrentPos);

	while (SimTime < MaxSimTime)
	{
		FVector OldPos = CurrentPos;

		// Integrate
		FVector Accel = GetArrowAcceleration(CurrentVel, GravityZ, Stats, Subsystem);
		CurrentVel += Accel * StepTime;
		CurrentPos += CurrentVel * StepTime;

		OutPathPoints.Add(CurrentPos);
		SimTime += StepTime;

		// Sweep
		// We use SphereTrace
		bool bHit = UKismetSystemLibrary::SphereTraceSingle(
			WorldContextObject,
			OldPos,
			CurrentPos,
			Radius,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			OutHit,
			true
		);

		if (bHit)
		{
			return true;
		}
	}

	return false;
}
