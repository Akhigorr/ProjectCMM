#include "ArcherComponent.h"
#include "BaseKillCamArrow.h"
#include "RealisticArrowMovementComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UArcherComponent::UArcherComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

ABaseKillCamArrow* UArcherComponent::FireArrow(TSubclassOf<ABaseKillCamArrow> ArrowClass, FVector SpawnLocation, FVector LaunchDirection, float Speed)
{
	if (!ArrowClass || !GetWorld()) return nullptr;

	AActor* Owner = GetOwner();
	if (!Owner) return nullptr;

	FRotator SpawnRotation = LaunchDirection.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Owner;
	SpawnParams.Instigator = Owner->GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn the arrow
	ABaseKillCamArrow* NewArrow = GetWorld()->SpawnActor<ABaseKillCamArrow>(ArrowClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (NewArrow)
	{
		// Set Velocity
		if (NewArrow->ArrowMovementComponent)
		{
			NewArrow->ArrowMovementComponent->Velocity = LaunchDirection.GetSafeNormal() * Speed;
			NewArrow->ArrowMovementComponent->UpdateComponentVelocity();
		}
	}

	return NewArrow;
}

void UArcherComponent::GetSocketLaunchTransform(USceneComponent* Parent, FName SocketName, FVector& OutLocation, FVector& OutDirection) const
{
	if (Parent)
	{
		FTransform SocketTransform = Parent->GetSocketTransform(SocketName);
		OutLocation = SocketTransform.GetLocation();
		OutDirection = SocketTransform.GetRotation().GetForwardVector();
	}
	else
	{
		OutLocation = FVector::ZeroVector;
		OutDirection = FVector::ForwardVector;
	}
}
