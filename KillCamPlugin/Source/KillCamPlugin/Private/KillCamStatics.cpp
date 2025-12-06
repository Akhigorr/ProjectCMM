#include "KillCamStatics.h"
#include "KillCamWorldSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

FArrowBallisticStats UKillCamStatics::GetArrowPreset(EArrowPreset Preset)
{
	FArrowBallisticStats Stats;

	switch (Preset)
	{
	case EArrowPreset::Standard:
		Stats.QuadraticDragCoefficient = 0.0001f;
		Stats.GravityScale = 1.0f;
		Stats.FletchingRotationSpeed = 360.0f;
		Stats.PenetrationDepth = 15.0f;
		Stats.bEnableBounce = true;
		break;

	case EArrowPreset::Heavy:
		Stats.QuadraticDragCoefficient = 0.0003f; // More drag
		Stats.GravityScale = 1.5f; // Falls faster
		Stats.FletchingRotationSpeed = 180.0f;
		Stats.PenetrationDepth = 25.0f; // Hits harder
		Stats.bEnableBounce = false;
		break;

	case EArrowPreset::Sniper:
		Stats.QuadraticDragCoefficient = 0.00005f; // Very little drag
		Stats.GravityScale = 0.5f; // Flatter arc
		Stats.FletchingRotationSpeed = 720.0f;
		Stats.PenetrationDepth = 30.0f;
		Stats.bEnableBounce = true;
		break;

	case EArrowPreset::Arcade:
		Stats.QuadraticDragCoefficient = 0.0f; // No drag
		Stats.GravityScale = 1.0f;
		Stats.FletchingRotationSpeed = 360.0f;
		Stats.PenetrationDepth = 10.0f;
		Stats.bEnableBounce = true;
		break;

	case EArrowPreset::Floater:
		Stats.QuadraticDragCoefficient = 0.0002f;
		Stats.GravityScale = 0.2f; // Moon gravity
		Stats.FletchingRotationSpeed = 100.0f;
		Stats.PenetrationDepth = 5.0f;
		Stats.bEnableBounce = true;
		break;
	}

	return Stats;
}

bool UKillCamStatics::PredictArrowPath(const UObject* WorldContextObject, FVector StartPos, FVector LaunchVelocity,
	float GravityScale, float DragCoefficient, FVector WindVector,
	float SimTime, float SimStep, float Radius,
	TArray<FVector>& OutPathPositions, FHitResult& OutHit)
{
	if (!WorldContextObject) return false;
	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return false;

	OutPathPositions.Empty();
	OutPathPositions.Add(StartPos);

	FVector CurrentPos = StartPos;
	FVector CurrentVel = LaunchVelocity;

	// Resolve Global Environment
	float GlobalGrav = 1.0f;
	float GlobalDrag = 1.0f;
	FVector GlobalWind = FVector::ZeroVector;

	if (const UKillCamWorldSubsystem* Subsystem = World->GetSubsystem<UKillCamWorldSubsystem>())
	{
		GlobalGrav = Subsystem->GetGlobalGravityScalar();
		GlobalDrag = Subsystem->GetGlobalDragModifier();
		GlobalWind = Subsystem->GetGlobalWind();
	}

	float EffectiveGravityZ = World->GetGravityZ() * GravityScale * GlobalGrav;
	float EffectiveDrag = DragCoefficient * GlobalDrag;
	FVector EffectiveWind = WindVector + GlobalWind;

	// Loop
	for (float t = 0; t < SimTime; t += SimStep)
	{
		FVector Acceleration = FVector(0, 0, EffectiveGravityZ);

		// Drag
		if (EffectiveDrag > 0.0f && !CurrentVel.IsZero())
		{
			float SpeedSq = CurrentVel.SizeSquared();
			Acceleration -= CurrentVel.GetSafeNormal() * (EffectiveDrag * SpeedSq);
		}

		// Wind
		Acceleration += EffectiveWind;

		// Integrate
		FVector NextPos = CurrentPos + (CurrentVel * SimStep) + (0.5f * Acceleration * SimStep * SimStep);
		FVector NewVel = CurrentVel + (Acceleration * SimStep);

		// Check Collision
		FCollisionQueryParams QueryParams;
		// QueryParams.AddIgnoredActor(Owner); // Cannot easily ignore owner in static func unless passed in

		if (World->SweepSingleByChannel(OutHit, CurrentPos, NextPos, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(Radius), QueryParams))
		{
			OutPathPositions.Add(OutHit.Location);
			return true; // Hit
		}

		CurrentPos = NextPos;
		CurrentVel = NewVel;
		OutPathPositions.Add(CurrentPos);
	}

	return false; // No hit in range
}

bool UKillCamStatics::SuggestTossVelocity(const UObject* WorldContextObject, FVector Start, FVector End, float TossSpeed, float GravityZ, FVector& OutVelocity, bool bHighArc)
{
	// Ensure we pass the World Context and the Override Gravity (GravityZ)
	// SuggestProjectileVelocity uses the 'OverrideGravityZ' parameter (last float) if it is not 0.
	// If 0, it uses World Gravity.

	return UGameplayStatics::SuggestProjectileVelocity(WorldContextObject, OutVelocity, Start, End, TossSpeed, bHighArc, 0.0f, GravityZ, ESuggestProjVelocityTraceOption::DoNotTrace);
}
