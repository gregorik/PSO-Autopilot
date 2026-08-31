// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PSOAutopilotCoreSettings.generated.h"

/**
 * Settings for the PSO Autopilot Core plugin.
 * Configures the memory-safe chunking and garbage collection constraints.
 */
UCLASS(Config=Game, defaultconfig, meta=(DisplayName="PSO Autopilot Core"))
class PSOAUTOPILOTCORE_API UPSOAutopilotCoreSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPSOAutopilotCoreSettings();

	/** Target directories to scan for materials to warm up (e.g., /Game, /Game/Characters, /Game/VFX). */
	UPROPERTY(EditAnywhere, Config, Category = "Targeting", meta = (ToolTip = "Directories to search for material assets. Subfolders are scanned recursively."))
	TArray<FDirectoryPath> DirectoriesToScan;

	/** Number of assets to load into memory at one time. Lower this to prevent Out-Of-Memory (OOM) crashes on constrained platforms. */
	UPROPERTY(EditAnywhere, Config, Category = "Memory Management", meta=(ClampMin="10", ClampMax="1000", UIMin="10", UIMax="500", ToolTip = "Max assets loaded concurrently in a single batch. Defaults to 100."))
	int32 BatchSize;

	/** If true, explicitly calls Garbage Collection after every batch is unloaded to ensure RAM stays flat. */
	UPROPERTY(EditAnywhere, Config, Category = "Memory Management", meta = (ToolTip = "Triggers non-blocking garbage collection after each batch is unloaded."))
	bool bGarbageCollectBetweenBatches;

	// Note: Core yields between batches, not within one, so there is no per-frame time budget to configure.
	// Intra-frame time-slicing of the game thread, smart cache skipping, and adaptive frame pacing are Pro features.
};

