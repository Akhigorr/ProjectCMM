#include "ArcTargetComponent.h"
#include "GameFramework/Actor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "KillCamWorldSubsystem.h"
#include "Engine/World.h"

UArcTargetComponent::UArcTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Health = 100.0f;
	ScoreValue = 10;
	bAutoFindGeometryCollection = true;
	bIsShattered = false;
}

void UArcTargetComponent::BeginPlay()
{
	Super::BeginPlay();

	// Bind to owner damage events
	AActor* Owner = GetOwner();
	if (Owner)
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UArcTargetComponent::OnOwnerTakeAnyDamage);
		Owner->OnTakePointDamage.AddDynamic(this, &UArcTargetComponent::OnOwnerTakePointDamage);
	}
}

void UArcTargetComponent::OnOwnerTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	// Fallback if Point Damage wasn't used
	TakeDamage(Damage, GetOwner()->GetActorLocation());
}

void UArcTargetComponent::OnOwnerTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
	TakeDamage(Damage, HitLocation);
}

void UArcTargetComponent::TakeDamage(float Amount, FVector ImpactPoint)
{
	if (bIsShattered) return;

	Health -= Amount;

	if (Health <= 0.0f)
	{
		Shatter(ImpactPoint);
	}
}

void UArcTargetComponent::Shatter(FVector ImpactPoint)
{
	if (bIsShattered) return;
	bIsShattered = true;

	// 1. Trigger Geometry Collection
	if (bAutoFindGeometryCollection && GetOwner())
	{
		if (UGeometryCollectionComponent* GC = GetOwner()->FindComponentByClass<UGeometryCollectionComponent>())
		{
			GC->SetSimulatePhysics(true);
			GC->SetCollisionProfileName("Destructible");

			// Apply Impulse
			float ImpulseStrength = 500.0f;
			float ImpulseRadius = 100.0f;
			GC->AddRadialImpulse(ImpactPoint, ImpulseRadius, ImpulseStrength, ERadialImpulseFalloff::RIF_Linear, true);
		}
	}

	// 2. Score
	if (GetWorld())
	{
		if (UKillCamWorldSubsystem* Subsystem = GetWorld()->GetSubsystem<UKillCamWorldSubsystem>())
		{
			Subsystem->AddScore(ScoreValue);
		}
	}

	// 3. Notify
	OnShattered.Broadcast();
}
