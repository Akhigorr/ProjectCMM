#include "BaseKillCamArrow.h"
#include "KillCamComponent.h"
#include "RealisticArrowMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ABaseKillCamArrow::ABaseKillCamArrow()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create root component (assuming it's usually a collision comp or mesh, but we use a scene root for the base)
	// Users inheriting this will likely attach their mesh to the RootComponent.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Create Movement Component
	ArrowMovementComponent = CreateDefaultSubobject<URealisticArrowMovementComponent>(TEXT("ArrowMovementComponent"));
	ArrowMovementComponent->UpdatedComponent = RootComponent;

	// Create Kill Cam Component
	KillCamComponent = CreateDefaultSubobject<UKillCamComponent>(TEXT("KillCamComponent"));

	// Create Camera Boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 150.0f; // Distance behind arrow
	CameraBoom->bUsePawnControlRotation = false; // Let the arrow rotation drive it
	CameraBoom->bEnableCameraLag = true; // Smooth follow as requested
	CameraBoom->CameraLagSpeed = 10.0f;

	// Create Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void ABaseKillCamArrow::BeginPlay()
{
	Super::BeginPlay();

	// Ensure the lag speed matches the component config if desired,
	// or let the component drive it in Tick if dynamic updates are needed.
	if (KillCamComponent && CameraBoom)
	{
		CameraBoom->CameraLagSpeed = KillCamComponent->CameraLagSpeed;
	}
}

void ABaseKillCamArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Dynamic updates if properties change at runtime
	if (KillCamComponent && CameraBoom)
	{
		// Could sync lag speed here if changed via UI
		// CameraBoom->CameraLagSpeed = KillCamComponent->CameraLagSpeed;
	}
}
