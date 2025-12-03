#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "RealisticArrowMovementComponent.generated.h"

/**
 * AAA Realistic Arrow Physics: Variable Drag, Wind, Spin, Oscillation, and Smart Ricochet.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KILLCAMPLUGIN_API URealisticArrowMovementComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()

public:
	URealisticArrowMovementComponent();

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta) override;
	virtual FVector ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const override;

public:
	// --- Aerodynamics ---

	/**
	 * Drag Coefficient based on air resistance.
	 * Standard Projectile drag is linear; this adds a quadratic element (Velocity^2).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Physics|Aerodynamics")
	float QuadraticDragCoefficient;

	/** Rotation speed of the arrow in degrees per second (Fletching stabilization). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Physics|Aerodynamics")
	float FletchingRotationSpeed;

	/** Global wind vector affecting the arrow trajectory. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Physics|Aerodynamics")
	FVector WindVector;

	// --- Archer's Paradox (Visual Oscillation) ---

	/** Initial amplitude of the arrow wiggle (in World Units) when fired. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Physics|Oscillation")
	float InitialOscillationAmplitude;

	/** Speed of the wiggle (Hz). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Physics|Oscillation")
	float OscillationFrequency;

	/** How fast the wiggle settles down (0-1 range approx). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Physics|Oscillation")
	float OscillationDamping;

	/**
	 * Current Oscillation Value (Read this in Blueprint to drive Material WPO).
	 * Values range from -Amplitude to +Amplitude and decay to 0.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Arrow Physics|Oscillation")
	float CurrentOscillationValue;

	// --- Impact & Ricochet ---

	/**
	 * If the impact angle (in degrees) is less than this, the arrow will ricochet/glance off.
	 * 0 = Perpendicular hit (Direct). 90 = Parallel hit.
	 * Example: 75.0 means if the arrow skims the wall at a 15 degree angle, it bounces.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Physics|Impact")
	float RicochetMaxAngle;

	/** How deep the arrow embeds itself into the target upon sticking (World Units). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Physics|Impact")
	float PenetrationDepth;

	/** If true, the arrow will attempt to ricochet off surfaces at shallow angles. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Physics|Impact")
	bool bEnableBounce;

	/** If true, the game will freeze momentarily upon impact (AAA feel). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Physics|Impact")
	bool bEnableHitStop;

	/** Duration of the hit stop in real-time seconds (e.g., 0.05). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Physics|Impact")
	float HitStopDuration;

protected:
	virtual void BeginPlay() override;

private:
	/** Time since initialization, used for oscillation calc */
	float TimeAlive;

	/** Cached mesh for spinning */
	TWeakObjectPtr<UPrimitiveComponent> CachedMeshToSpin;

	/** Stored time dilation to restore after hit stop */
	float PreHitStopTimeDilation;

	/** Helper to handle sticking logic */
	void StickToTarget(const FHitResult& Hit);

	/** Trigger the hit stop effect */
	void PerformHitStop();

	/** End the hit stop effect */
	void StopHitStop();

	FTimerHandle TimerHandle_HitStop;
};
