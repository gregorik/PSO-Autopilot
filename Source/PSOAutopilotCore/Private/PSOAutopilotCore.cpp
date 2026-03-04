// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#include "PSOAutopilotCore.h"

DEFINE_LOG_CATEGORY(LogPSOAutopilotCore);

void FPSOAutopilotCoreModule::StartupModule()
{
	UE_LOG(LogPSOAutopilotCore, Log, TEXT("PSO Autopilot Module Started."));
}

void FPSOAutopilotCoreModule::ShutdownModule()
{
	UE_LOG(LogPSOAutopilotCore, Log, TEXT("PSO Autopilot Module Shutdown."));
}

IMPLEMENT_MODULE(FPSOAutopilotCoreModule, PSOAutopilotCore)
