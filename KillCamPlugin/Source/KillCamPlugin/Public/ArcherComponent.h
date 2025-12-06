#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArcherComponent.generated.h"

class ABaseKillCamArrow;

/**
 * Component to handle spawning and initializing Kill Cam Arrows.
 * Attaching this to a Pawn ensures arrows are spawned with the correct Owner/Instigator setup.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KILLCAMPLUGIN_API UArcherComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UArcherComponent();

	/**
	 * Spawns an arrow actor and initializes its velocity.
	 * @param ArrowClass The class of arrow to spawn (must inherit ABaseKillCamArrow).
	 * @param SpawnLocation World location to spawn at.
	 * @param LaunchDirection Normalized direction vector.
	 * @param Speed Launch speed (Units/Sec).
	 * @return The spawned arrow actor (or null if failed).
	 */
	UFUNCTION(BlueprintCallable, Category = "Archer")
	ABaseKillCamArrow* FireArrow(TSubclassOf<ABaseKillCamArrow> ArrowClass, FVector SpawnLocation, FVector LaunchDirection, float Speed);

	/**
	 * Helper to calculate launch location/rotation based on a Socket or Component.
	 * @param Parent Parent component (Mesh, Weapon, etc).
	 * @param SocketName Socket to use.
	 * @param OutLocation Resulting Location.
	 * @param OutDirection Resulting Forward Vector.
	 */
	UFUNCTION(BlueprintPure, Category = "Archer")
	void GetSocketLaunchTransform(USceneComponent* Parent, FName SocketName, FVector& OutLocation, FVector& OutDirection) const;
};
