#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KillCamComponent.generated.h"

// Forward declarations to avoid including Engine headers in Public
class UCurveFloat;

UENUM(BlueprintType)
enum class EKillCamMode : uint8
{
	Realtime	UMETA(DisplayName = "Realtime Follow"),
	Predictive	UMETA(DisplayName = "Predictive Trigger")
};

UENUM(BlueprintType)
enum class ELookAheadMethod : uint8
{
	Trace		UMETA(DisplayName = "Line Trace"),
	Physics		UMETA(DisplayName = "Physics Prediction")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnKillCamUpdate, float, DistanceToTarget, float, SpeedRatio);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnKillCamTrigger);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KILLCAMPLUGIN_API UKillCamComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKillCamComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// --- Configuration ---

	/** Which mode the kill cam operates in */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill Cam|Config")
	EKillCamMode KillCamMode;

	/** Method used for looking ahead */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill Cam|Config")
	ELookAheadMethod LookAheadMethod;

	/** Tag to look for on potential targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill Cam|Config")
	FName TargetTag;

	/** How much to slow down time when Kill Cam is active (0.1 = 10% speed) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill Cam|Visuals", meta = (ClampMin = "0.001", ClampMax = "1.0"))
	float TargetTimeDilation;

	/** Curve to drive time dilation transition over time (optional) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill Cam|Visuals")
	TObjectPtr<UCurveFloat> TimeDilationCurve;

	/** How smoothly the camera follows the arrow (Lag speed) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill Cam|Visuals")
	float CameraLagSpeed;

	/** Maximum distance to look ahead for targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill Cam|Prediction")
	float LookAheadDistance;

	/** Radius for sphere trace/prediction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill Cam|Prediction")
	float PredictionRadius;

	/** Debug: If true, TriggerLookAhead will always succeed (force start) without hitting anything. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill Cam|Debug")
	bool bDebugForceAlwaysTrigger;

	// --- Camera Control ---

	/** If true, the plugin will automatically take control of the Player Camera. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill Cam|Camera Control")
	bool bAutoSwitchView;

	/** How long to wait after the arrow stops (impact) before returning control to the player. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill Cam|Camera Control")
	float PostImpactDelay;

	/** Blend time when switching TO the kill cam. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill Cam|Camera Control")
	float BlendToCamTime;

	/** Blend time when switching BACK to the player. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kill Cam|Camera Control")
	float BlendBackTime;

	// --- Events ---

	/** Fired every tick while Kill Cam is active, useful for driving Post Process Materials */
	UPROPERTY(BlueprintAssignable, Category = "Kill Cam|Events")
	FOnKillCamUpdate OnKillCamUpdate;

	/** Fired when the Kill Cam actually starts (slow mo engages) */
	UPROPERTY(BlueprintAssignable, Category = "Kill Cam|Events")
	FOnKillCamTrigger OnKillCamStart;

	/** Fired when the Kill Cam ends */
	UPROPERTY(BlueprintAssignable, Category = "Kill Cam|Events")
	FOnKillCamTrigger OnKillCamEnd;

	// --- Functions ---

	/** Manually trigger the look ahead check. Returns true if a valid target was found. */
	UFUNCTION(BlueprintCallable, Category = "Kill Cam")
	bool TriggerLookAhead();

	/** Force start the kill cam effect. If bAutoSwitchView is true, it switches the camera. */
	UFUNCTION(BlueprintCallable, Category = "Kill Cam")
	void StartKillCam();

	/** Stop the kill cam effect and return camera control. */
	UFUNCTION(BlueprintCallable, Category = "Kill Cam")
	void StopKillCam();

private:
	/** Tracks if the kill cam logic is currently running */
	bool bIsKillCamActive;

	/** Timer handle for the post-impact delay */
	FTimerHandle TimerHandle_StopCam;

	/** Helper to perform the specific prediction logic */
	bool PerformPrediction(FHitResult& OutHit);

	/** Cached reference to the owner as an Actor */
	TWeakObjectPtr<AActor> OwnerActor;

	/** The view target we had before switching */
	TWeakObjectPtr<AActor> OriginalViewTarget;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
