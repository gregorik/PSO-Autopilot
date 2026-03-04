// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PSOAutopilotCoreSettings.generated.h"

/**
 * Settings for the PSO Autopilot plugin.
 * Configures the memory-safe chunking and time-slicing constraints.
 */
UCLASS(Config=Game, defaultconfig, meta=(DisplayName="PSO Autopilot"))
class PSOAUTOPILOTCORE_API UPSOAutopilotCoreSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPSOAutopilotCoreSettings();

	/** Target directories to scan for materials and systems to warm up. (e.g., /Game/Characters) */
	UPROPERTY(EditAnywhere, Config, Category = "Targeting")
	TArray<FDirectoryPath> DirectoriesToScan;

	/** Number of assets to load into memory at one time. Lower this to prevent Out-Of-Memory (OOM) crashes. */
	UPROPERTY(EditAnywhere, Config, Category = "Memory Management", meta=(ClampMin="10", ClampMax="1000"))
	int32 BatchSize;

	/** If true, explicitly calls Garbage Collection after every batch is unloaded to ensure RAM stays low. */
	UPROPERTY(EditAnywhere, Config, Category = "Memory Management")
	bool bGarbageCollectBetweenBatches;

	/** The maximum amount of time (in milliseconds) the plugin is allowed to spend compiling shaders per frame. Keeps loading screens smooth. */
	UPROPERTY(EditAnywhere, Config, Category = "Performance (Seamless UI)", meta=(ClampMin="0.5", ClampMax="33.0"))
	float MaxProcessingTimeMsPerFrame;
};
