// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#include "PSOAutopilotCoreSettings.h"

UPSOAutopilotCoreSettings::UPSOAutopilotCoreSettings()
{
	// Default Settings tailored for AAA stability
	BatchSize = 100;
	bGarbageCollectBetweenBatches = true;
	MaxProcessingTimeMsPerFrame = 5.0f; // 5ms leaves plenty of frame budget for UI animations (60fps is 16.6ms)
	
	// Default to Game root
	FDirectoryPath RootPath;
	RootPath.Path = TEXT("/Game");
	DirectoriesToScan.Add(RootPath);
}
