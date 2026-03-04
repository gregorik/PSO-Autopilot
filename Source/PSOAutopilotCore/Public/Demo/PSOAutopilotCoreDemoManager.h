// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PSOAutopilotCoreDemoManager.generated.h"

class UPSOAutopilotCoreDemoWidget;

/**
 * Orchestrates the PSO Autopilot Demo level, spawning the UI and starting the warmup sequence.
 */
UCLASS()
class PSOAUTOPILOTCORE_API APSOAutopilotCoreDemoManager : public AActor
{
	GENERATED_BODY()
	
public:	
	APSOAutopilotCoreDemoManager();

protected:
	virtual void BeginPlay() override;

public:
	/** The class of the HUD to spawn. If empty, a default C++ UI will be generated. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
	TSubclassOf<class UUserWidget> HUDClass;

private:
	UPROPERTY()
	TObjectPtr<UPSOAutopilotCoreDemoWidget> ActiveHUD;
};
