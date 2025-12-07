#include "BaseKillCamArrow.h"
#include "KillCamComponent.h"
#include "RealisticArrowMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"

ABaseKillCamArrow::ABaseKillCamArrow()
{
	PrimaryActorTick.bCanEverTick = true;

	// Use a SphereComponent as root so projectile movement can sweep for collisions.
	USphereComponent* CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->SetUseCCD(true); // Prevent tunneling at high speeds
	RootComponent = CollisionComp;

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
	bEnableCameraCollision = true;
	CameraProbeSize = 12.0f;
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

	// Collision Safety: Ignore the owner (Instigator) so we don't hit ourselves immediately
	if (GetOwner())
	{
		// MoveIgnoreActor adds the owner to the IgnoreActors list for all primitive components
		if (UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(RootComponent))
		{
			RootPrim->MoveIgnoreActor(GetOwner());
		}

		// Explicitly ensure the movement component knows (if it has specific ignore logic)
		if (KillCamComponent)
		{
			// Note: KillCamComponent uses OwnerActor for prediction, which is already GetOwner()
		}
	}

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

	// Apply Collision Settings
	CameraBoom->bDoCollisionTest = bEnableCameraCollision;
	CameraBoom->ProbeSize = CameraProbeSize;

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
