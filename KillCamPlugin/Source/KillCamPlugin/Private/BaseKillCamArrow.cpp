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

	// Defaults
	FramingMode = EKillCamFramingMode::StandardRear;
	KillCamFOV = 90.0f;
}

void ABaseKillCamArrow::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyFramingPreset();
}

void ABaseKillCamArrow::BeginPlay()
{
	Super::BeginPlay();

	ApplyFramingPreset();

	// Ensure the lag speed matches the component config if desired,
	// or let the component drive it in Tick if dynamic updates are needed.
	if (KillCamComponent && CameraBoom)
	{
		CameraBoom->CameraLagSpeed = KillCamComponent->CameraLagSpeed;
	}
}

void ABaseKillCamArrow::ApplyFramingPreset()
{
	if (!CameraBoom || !FollowCamera) return;

	FollowCamera->SetFieldOfView(KillCamFOV);

	switch (FramingMode)
	{
	case EKillCamFramingMode::StandardRear:
		CameraBoom->TargetArmLength = 150.0f;
		CameraBoom->SocketOffset = FVector(0.f, 0.f, 20.f);
		CameraBoom->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));
		break;

	case EKillCamFramingMode::CinematicSide:
		CameraBoom->TargetArmLength = 100.0f;
		CameraBoom->SocketOffset = FVector(0.f, 60.f, 10.f);
		CameraBoom->SetRelativeRotation(FRotator(-5.f, 0.f, 0.f));
		break;

	case EKillCamFramingMode::TopDown:
		CameraBoom->TargetArmLength = 300.0f;
		CameraBoom->SocketOffset = FVector(0.f, 0.f, 0.f);
		CameraBoom->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
		break;

	case EKillCamFramingMode::Custom:
		// Do nothing, let user manual settings persist
		break;
	}
}

void ABaseKillCamArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Dynamic updates if properties change at runtime (allows tweaking in Editor)
	if (KillCamComponent && CameraBoom)
	{
		CameraBoom->CameraLagSpeed = KillCamComponent->CameraLagSpeed;
	}
}
