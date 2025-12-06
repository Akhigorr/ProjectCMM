#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ArcTarget_GhostInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UArcTarget_GhostInterface : public UInterface
{
	GENERATED_BODY()
};

class KILLCAMPLUGIN_API IArcTarget_GhostInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ghost Target")
	void SetGhostVisibility(bool bVisible);
};
