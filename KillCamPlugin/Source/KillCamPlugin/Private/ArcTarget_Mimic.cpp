#include "ArcTarget_Mimic.h"
#include "Components/SphereComponent.h"
#include "RealisticArrowMovementComponent.h"

AArcTarget_Mimic::AArcTarget_Mimic()
{
	DetectionRadius = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionRadius"));
	DetectionRadius->SetupAttachment(RootComponent);
	DetectionRadius->InitSphereRadius(500.0f);
	DetectionRadius->OnComponentBeginOverlap.AddDynamic(this, &AArcTarget_Mimic::OnOverlapBegin);

	DodgeSpeed = 5.0f;
	DodgeDistance = 300.0f;
	bIsDodging = false;
}

void AArcTarget_Mimic::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		// Only dodge projectiles that have our realistic movement component
		if (!OtherActor->FindComponentByClass<URealisticArrowMovementComponent>())
		{
			return;
		}

		FVector DirectionToProjectile = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();

		// Dodge perpendicular + slightly away
		FVector Right = FVector::CrossProduct(DirectionToProjectile, FVector::UpVector);
		if (FMath::RandBool()) Right *= -1.0f; // Randomize Left/Right

		TargetDodgeLocation = GetActorLocation() + (Right * DodgeDistance);
		bIsDodging = true;
	}
}

void AArcTarget_Mimic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDodging && !bIsShattered)
	{
		FVector NewLoc = FMath::VInterpTo(GetActorLocation(), TargetDodgeLocation, DeltaTime, DodgeSpeed);
		SetActorLocation(NewLoc);

		if (FVector::DistSquared(NewLoc, TargetDodgeLocation) < 100.0f)
		{
			bIsDodging = false;
		}
	}
}
