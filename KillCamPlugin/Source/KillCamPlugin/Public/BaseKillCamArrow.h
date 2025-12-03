#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseKillCamArrow.generated.h"

class UKillCamComponent;
class URealisticArrowMovementComponent;
class USpringArmComponent;
class UCameraComponent;

UENUM(BlueprintType)
enum class EKillCamFramingMode : uint8
{
	StandardRear	UMETA(DisplayName = "Standard Rear"),
	CinematicSide	UMETA(DisplayName = "Cinematic Side"),
	TopDown			UMETA(DisplayName = "Top Down"),
	Custom			UMETA(DisplayName = "Custom")
};

UCLASS()
class KILLCAMPLUGIN_API ABaseKillCamArrow : public AActor
{
	GENERATED_BODY()

public:
	ABaseKillCamArrow();
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// --- Framing Options ---

	/** Choose a preset camera position/angle. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill Cam|Framing")
	EKillCamFramingMode FramingMode;

	/** Field of view for the kill cam. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill Cam|Framing")
	float KillCamFOV;

	// --- Collision / Obstacle Avoidance ---

	/** If true, the camera will pull in closer to avoid clipping through walls. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill Cam|Framing")
	bool bEnableCameraCollision;

	/** Size of the probe (sphere) used for camera collision. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill Cam|Framing")
	float CameraProbeSize;

	/** Helper to apply the chosen framing mode to the spring arm/camera. */
	UFUNCTION(BlueprintCallable, Category = "Kill Cam|Framing")
	void ApplyFramingPreset();

	// --- Components ---

	/** Realistic Movement Component (Physics, Drag, Ricochet) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kill Cam|Physics")
	TObjectPtr<URealisticArrowMovementComponent> ArrowMovementComponent;

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
