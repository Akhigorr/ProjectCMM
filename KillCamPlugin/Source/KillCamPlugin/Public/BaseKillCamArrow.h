#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseKillCamArrow.generated.h"

class UKillCamComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class KILLCAMPLUGIN_API ABaseKillCamArrow : public AActor
{
	GENERATED_BODY()

public:
	ABaseKillCamArrow();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// --- Components ---

	/** The core Kill Cam logic component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kill Cam")
	TObjectPtr<UKillCamComponent> KillCamComponent;

	/** Spring arm to hold the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** The camera that will render the kill cam view */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

};
