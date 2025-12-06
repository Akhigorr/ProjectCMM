#include "RealisticArrowMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/ShapeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "KillCamWorldSubsystem.h"
#include "KillCamPlugin.h"
#include "KillCamStatics.h"

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

FArrowBallisticStats URealisticArrowMovementComponent::GetCurrentBallisticStats() const
{
	FArrowBallisticStats Stats;
	Stats.QuadraticDragCoefficient = QuadraticDragCoefficient;
	Stats.GravityScale = ProjectileGravityScale;
	Stats.FletchingRotationSpeed = FletchingRotationSpeed;
	Stats.PenetrationDepth = PenetrationDepth;
	Stats.bEnableBounce = bEnableBounce;
	return Stats;
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

	// Cache Subsystem
	if (GetWorld())
	{
		CachedSubsystem = GetWorld()->GetSubsystem<UKillCamWorldSubsystem>();
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
	float GravityZ = GetGravityZ();
	FArrowBallisticStats Stats = GetCurrentBallisticStats();

	// Use shared logic
	return UKillCamStatics::GetArrowAcceleration(InVelocity, GravityZ, Stats, CachedSubsystem.Get());
}

void URealisticArrowMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	// Calculate Impact Angle
	FVector Forward = Velocity.GetSafeNormal();
	float CosAngle = FVector::DotProduct(Hit.Normal, -Forward);
	float ImpactAngleDeg = FMath::RadiansToDegrees(FMath::Acos(CosAngle)); // 0 to 90

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

	// Apply Damage to the target so it can react (e.g. AArcTarget::Shatter)
	if (Hit.GetActor())
	{
		UGameplayStatics::ApplyPointDamage(
			Hit.GetActor(),
			100.0f, // Base Damage
			ForwardDir,
			Hit,
			GetOwner()->GetInstigatorController(),
			GetOwner(),
			nullptr // DamageType class
		);
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
	// 1. Request Time Dilation via Subsystem (0.001f is hard limit)
	if (CachedSubsystem.IsValid())
	{
		CachedSubsystem->RequestTimeDilation("HitStop", 0.001f);
	}

	// 2. Set Timer for Next Tick loop to count down real-time
	RemainingHitStopDuration = HitStopDuration;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &URealisticArrowMovementComponent::OnHitStopNextTick);
	}
}

void URealisticArrowMovementComponent::OnHitStopNextTick()
{
	// Consume real time
	if (GetWorld())
	{
		float RealDelta = GetWorld()->GetRealTimeSeconds() - GetWorld()->GetTimeSeconds();
		// Wait, GetTimeSeconds is dilated. GetRealTimeSeconds is monotonic real time? No.
		// World->GetDeltaSeconds() is game time. World->GetRealTimeSeconds() is usually platform time.
		// But DeltaSeconds is easier if we just divide by dilation?
		// Actually, GetWorld()->GetDeltaSeconds() * (1/Dilation) is approx Real Time.
		// Better: use direct platform time difference if we tracked it, OR just assume 1 frame is small enough?
		// User specifically asked for SetTimerForNextTick for "sub-frame" or "next-tick" delays logic.
		// But we need to wait for X real seconds.

		// Let's use `UGameplayStatics::GetWorldDeltaSeconds` (dilated) / Dilation to get RealDelta.
		float Dilation = UGameplayStatics::GetGlobalTimeDilation(this);
		if (Dilation < SMALL_NUMBER) Dilation = 0.001f;

		float DT = GetWorld()->GetDeltaSeconds();
		float RealDT = DT / Dilation;

		RemainingHitStopDuration -= RealDT;

		if (RemainingHitStopDuration <= 0.0f)
		{
			StopHitStop();
		}
		else
		{
			// Continue loop
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &URealisticArrowMovementComponent::OnHitStopNextTick);
		}
	}
}

void URealisticArrowMovementComponent::StopHitStop()
{
	if (CachedSubsystem.IsValid())
	{
		CachedSubsystem->ClearTimeDilationRequest("HitStop");
	}
}
