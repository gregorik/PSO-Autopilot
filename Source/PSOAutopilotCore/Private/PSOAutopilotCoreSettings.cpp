// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#include "PSOAutopilotCoreSettings.h"

UPSOAutopilotCoreSettings::UPSOAutopilotCoreSettings()
{
	// Conservative defaults: small enough batches that peak memory stays flat on large projects.
	BatchSize = 100;
	bGarbageCollectBetweenBatches = true;


	// Default to Game root
	FDirectoryPath RootPath;
	RootPath.Path = TEXT("/Game");
	DirectoriesToScan.Add(RootPath);
}
