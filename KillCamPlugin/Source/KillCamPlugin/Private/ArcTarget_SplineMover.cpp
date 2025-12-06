#include "ArcTarget_SplineMover.h"
#include "Components/SplineComponent.h"

AArcTarget_SplineMover::AArcTarget_SplineMover()
{
	MoveSpeed = 200.0f;
	bPingPong = true;
	DistanceAlongSpline = 0.0f;
	Direction = 1;
}

void AArcTarget_SplineMover::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsShattered || !SplinePathActor) return;

	USplineComponent* SplineComp = SplinePathActor->FindComponentByClass<USplineComponent>();
	if (SplineComp)
	{
		float SplineLength = SplineComp->GetSplineLength();
		DistanceAlongSpline += (MoveSpeed * DeltaTime * Direction);

		if (DistanceAlongSpline >= SplineLength)
		{
			if (bPingPong)
			{
				DistanceAlongSpline = SplineLength;
				Direction = -1;
			}
			else
			{
				DistanceAlongSpline = 0.0f;
			}
		}
		else if (DistanceAlongSpline <= 0.0f && Direction == -1)
		{
			DistanceAlongSpline = 0.0f;
			Direction = 1;
		}

		FVector NewLoc = SplineComp->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
		FRotator NewRot = SplineComp->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);

		SetActorLocationAndRotation(NewLoc, NewRot);
	}
}
