#include "RealisticArrowMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/ShapeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "KillCamWorldSubsystem.h"
#include "KillCamPlugin.h"

URealisticArrowMovementComponent::URealisticArrowMovementComponent()
{
	// Defaults for an "Arrow" feel
	bRotationFollowsVelocity = true;
	ProjectileGravityScale = 1.0f; // Standard gravity
	QuadraticDragCoefficient = 0.0001f; // Slight air resistance
	FletchingRotationSpeed = 360.0f; // 1 full spin per second
	WindVector = FVector::ZeroVector;

	InitialOscillationAmplitude = 5.0f;
	OscillationFrequency = 15.0f;
	OscillationDamping = 2.0f;

	RicochetMaxAngle = 70.0f; // Glancing hits will bounce
	PenetrationDepth = 15.0f;

	bShouldBounce = true; // We handle the decision in HandleImpact
	bEnableBounce = true;
	Bounciness = 0.3f;
	Friction = 0.5f;

	// Hit Stop
	bEnableHitStop = true;
	HitStopDuration = 0.05f;

	TimeAlive = 0.0f;
}

void URealisticArrowMovementComponent::ApplyBallisticStats(const FArrowBallisticStats& Stats)
{
	QuadraticDragCoefficient = Stats.QuadraticDragCoefficient;
	ProjectileGravityScale = Stats.GravityScale;
	FletchingRotationSpeed = Stats.FletchingRotationSpeed;
	PenetrationDepth = Stats.PenetrationDepth;
	bEnableBounce = Stats.bEnableBounce;
}

void URealisticArrowMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// Cache the mesh for spinning
	AActor* Owner = GetOwner();
	if (Owner)
	{
		TArray<UPrimitiveComponent*> Comps;
		Owner->GetComponents(Comps);
		for (UPrimitiveComponent* Comp : Comps)
		{
			// Find first visual mesh that isn't the root/collider
			if (Comp != UpdatedComponent && !Comp->IsA<UShapeComponent>())
			{
				CachedMeshToSpin = Comp;
				break;
			}
		}
	}
}

void URealisticArrowMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Velocity.SizeSquared() > 1.0f)
	{
		// --- Fletching Rotation (Spin) ---
		if (CachedMeshToSpin.IsValid())
		{
			CachedMeshToSpin->AddLocalRotation(FRotator(0.0f, 0.0f, FletchingRotationSpeed * DeltaTime));
		}
	}

    // --- Archer's Paradox Oscillation ---
    // Formula: Amplitude * e^(-damping * t) * cos(freq * t)
    if (InitialOscillationAmplitude > 0.0f)
    {
        TimeAlive += DeltaTime;
        float Envelope = FMath::Exp(-OscillationDamping * TimeAlive);
        CurrentOscillationValue = InitialOscillationAmplitude * Envelope * FMath::Cos(OscillationFrequency * TimeAlive);
    }
}

FVector URealisticArrowMovementComponent::ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const
{
	// Calculate Gravity Scale (Local * Global)
	float FinalGravity = ProjectileGravityScale;
	float FinalDrag = QuadraticDragCoefficient;
	FVector FinalWind = WindVector;

	const UWorld* World = GetWorld();
	if (World)
	{
		if (const UKillCamWorldSubsystem* Subsystem = World->GetSubsystem<UKillCamWorldSubsystem>())
		{
			FinalGravity *= Subsystem->GetGlobalGravityScalar();
			FinalDrag *= Subsystem->GetGlobalDragModifier();
			FinalWind += Subsystem->GetGlobalWind();
		}
	}

	FVector Acceleration = FVector::ZeroVector;

	// Super implementation calculates: Acceleration.Z = GetGravityZ() * ProjectileGravityScale;
	Acceleration = Super::ComputeAcceleration(InVelocity, DeltaTime);

	// Apply Global Gravity Scalar from Subsystem
	if (World)
	{
		float GlobalGrav = 1.0f;
		if (const UKillCamWorldSubsystem* Subsystem = World->GetSubsystem<UKillCamWorldSubsystem>())
		{
			GlobalGrav = Subsystem->GetGlobalGravityScalar();
		}

		if (!FMath::IsNearlyEqual(GlobalGrav, 1.0f))
		{
			// Explicitly calculate the difference to ensure Total = Gravity * Scale * Global
			float BaseGravityZ = GetGravityZ() * ProjectileGravityScale;
			float TargetGravityZ = BaseGravityZ * GlobalGrav;

			// We add the difference because Super already added BaseGravityZ
			Acceleration.Z += (TargetGravityZ - BaseGravityZ);
		}
	}

	// --- Quadratic Drag ---
	// Force = -C * v^2 * direction
	if (FinalDrag > 0.0f && !InVelocity.IsZero())
	{
		float SpeedSq = InVelocity.SizeSquared();
		FVector DragForce = -InVelocity.GetSafeNormal() * (FinalDrag * SpeedSq);
		Acceleration += DragForce;
	}

	// --- Wind ---
	if (!FinalWind.IsZero())
	{
		Acceleration += FinalWind;
	}

	return Acceleration;
}

void URealisticArrowMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	// Calculate Impact Angle
	// Normal is perpendicular to surface. Velocity is incoming.
	// Dot(Normal, -Forward) gives Cos(Angle) where 1.0 is direct hit (0 deg), 0.0 is graze (90 deg).
	FVector Forward = Velocity.GetSafeNormal();
	float CosAngle = FVector::DotProduct(Hit.Normal, -Forward);
	float ImpactAngleDeg = FMath::RadiansToDegrees(FMath::Acos(CosAngle)); // 0 to 90

	// User defined "RicochetMaxAngle" is defined as deviation from Normal?
	// Let's interpret user setting:
	// If User says "RicochetMaxAngle = 70", it means if the hit is within 70 degrees of the normal (mostly direct), it sticks.
	// If it is > 70 (glancing, 70-90), it bounces.
	// Wait, standard convention: 0 is head on. 90 is parallel.

	bool bIsGlancing = ImpactAngleDeg > RicochetMaxAngle;

	if (bIsGlancing && bShouldBounce && bEnableBounce)
	{
		UE_LOG(LogKillCam, Verbose, TEXT("Arrow Physics: Ricochet off %s (Angle: %f)"), *Hit.GetActor()->GetName(), ImpactAngleDeg);
		OnArrowImpact.Broadcast(Hit, true);
		// Standard bounce
		Super::HandleImpact(Hit, TimeSlice, MoveDelta);
	}
	else
	{
		UE_LOG(LogKillCam, Verbose, TEXT("Arrow Physics: Stick into %s (Angle: %f)"), *Hit.GetActor()->GetName(), ImpactAngleDeg);
		OnArrowImpact.Broadcast(Hit, false);
		// Stick!
		StickToTarget(Hit);
	}
}

void URealisticArrowMovementComponent::StickToTarget(const FHitResult& Hit)
{
	// Cache direction before stopping (Velocity becomes zero)
	FVector ForwardDir = Velocity.GetSafeNormal();
	if (ForwardDir.IsZero()) ForwardDir = GetOwner()->GetActorForwardVector();

	if (bEnableHitStop)
	{
		PerformHitStop();
	}

	StopMovementImmediately();

	// Disable further physics/collision
	if (UpdatedComponent)
	{
		if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(UpdatedComponent))
		{
			Primitive->SetSimulatePhysics(false);
			Primitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	// Penetration: Move forward slightly
	FVector StickLocation = Hit.Location + (ForwardDir * PenetrationDepth);

	// Snap to hit location/rotation
	AActor* HitActor = Hit.GetActor();
	UPrimitiveComponent* HitComp = Hit.GetComponent();

	if (HitActor && HitComp)
	{
		// Attach to the component we hit so we move with it (e.g. moving enemy)
		UpdatedComponent->SetWorldLocation(StickLocation);
		UpdatedComponent->AttachToComponent(HitComp, FAttachmentTransformRules::KeepWorldTransform, Hit.BoneName);
	}
}

void URealisticArrowMovementComponent::PerformHitStop()
{
	// Cache current dilation (in case kill cam is active)
	PreHitStopTimeDilation = UGameplayStatics::GetGlobalTimeDilation(this);

	// Set to extremely low (nearly stopped)
	float HitStopScale = 0.001f;
	UGameplayStatics::SetGlobalTimeDilation(this, HitStopScale);

	// Timer needs to account for dilation.
	// Dilation = 0.001.
	// If we want 0.05 real seconds, the game time that passes is 0.05 * 0.001.
	float Delay = HitStopDuration * HitStopScale;

	// Remove arbitrary clamp to 0.0001f which can cause frame-rate issues
	// TimerManager handles small delays by firing next tick if it's too small
	if (Delay <= KINDA_SMALL_NUMBER)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &URealisticArrowMovementComponent::StopHitStop);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_HitStop, this, &URealisticArrowMovementComponent::StopHitStop, Delay, false);
	}
}

void URealisticArrowMovementComponent::StopHitStop()
{
	// Only restore if the current dilation is still our HitStop value (0.001f).
	// If it changed (e.g., KillCam ended and set it to 1.0f), we respect that change.
	float CurrentDilation = UGameplayStatics::GetGlobalTimeDilation(this);
	if (FMath::IsNearlyEqual(CurrentDilation, 0.001f, 0.0001f))
	{
		UGameplayStatics::SetGlobalTimeDilation(this, PreHitStopTimeDilation);
	}
}
