#pragma once

#include "CoreMinimal.h"
#include "ArcTarget.h"
#include "ArcTarget_GhostInterface.h"
#include "ArcTarget_Ghost.generated.h"

/**
 * Only collides when visible to the lens.
 */
UCLASS()
class KILLCAMPLUGIN_API AArcTarget_Ghost : public AArcTarget, public IArcTarget_GhostInterface
{
	GENERATED_BODY()

public:
	AArcTarget_Ghost();

	virtual void SetGhostVisibility_Implementation(bool bVisible) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Target|Ghost")
	bool bIsVisibleToLens;
};
