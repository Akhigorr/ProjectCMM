#include "ArcTarget_Orbiter.h"

AArcTarget_Orbiter::AArcTarget_Orbiter()
{
	RotationSpeed = 50.0f;
	Radius = 200.0f;
	CurrentAngle = 0.0f;
}

void AArcTarget_Orbiter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsShattered) return;

	if (PivotActor)
	{
		CurrentAngle += RotationSpeed * DeltaTime;
		CurrentAngle = FMath::Fmod(CurrentAngle, 360.0f);

		FVector PivotLoc = PivotActor->GetActorLocation();
		FVector Offset = FVector(FMath::Cos(FMath::DegreesToRadians(CurrentAngle)) * Radius, FMath::Sin(FMath::DegreesToRadians(CurrentAngle)) * Radius, 0.0f);

		SetActorLocation(PivotLoc + Offset);
	}
}
