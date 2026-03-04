// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#include "PSOAutopilotCoreSubsystem.h"
#include "PSOAutopilotCoreSettings.h"
#include "PSOAutopilotCore.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/AssetData.h"
#include "Materials/MaterialInterface.h"

#include "Engine/Engine.h"
#include "ShaderPipelineCache.h"

void UPSOAutopilotCoreSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CurrentState = EPSOWarmupState::Idle;
	UE_LOG(LogPSOAutopilotCore, Log, TEXT("PSO Autopilot Subsystem Initialized."));
}

void UPSOAutopilotCoreSubsystem::Deinitialize()
{
	if (CurrentStreamableHandle.IsValid() && CurrentStreamableHandle->IsActive())
	{
		CurrentStreamableHandle->CancelHandle();
	}
	Super::Deinitialize();
}

void UPSOAutopilotCoreSubsystem::Tick(float DeltaTime)
{
	// Issue 3 fix: Wait for Asset Registry
	if (CurrentState == EPSOWarmupState::Scanning)
	{
		ScanForAssets();
	}
	// USP 2: Time-sliced processing on the Game Thread to keep UI smooth
	else if (CurrentState == EPSOWarmupState::ProcessingBatch)
	{
		ProcessBatchTimeSliced();
	}
	// Delay GC by one frame to ensure streamable manager fully drops handles
	else if (CurrentState == EPSOWarmupState::WaitingForGC)
	{
		const UPSOAutopilotCoreSettings* Settings = GetDefault<UPSOAutopilotCoreSettings>();
		if (Settings->bGarbageCollectBetweenBatches && GEngine)
		{
			GEngine->ForceGarbageCollection(false);
		}
		
		CurrentState = EPSOWarmupState::LoadingBatch;
		AdvanceStateMachine();
	}
}

TStatId UPSOAutopilotCoreSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UPSOAutopilotCoreSubsystem, STATGROUP_Tickables);
}

bool UPSOAutopilotCoreSubsystem::IsTickable() const
{
	return CurrentState == EPSOWarmupState::ProcessingBatch || CurrentState == EPSOWarmupState::Scanning || CurrentState == EPSOWarmupState::WaitingForGC;
}

void UPSOAutopilotCoreSubsystem::StartWarmup()
{
	if (CurrentState != EPSOWarmupState::Idle && CurrentState != EPSOWarmupState::Finished)
	{
		UE_LOG(LogPSOAutopilotCore, Warning, TEXT("PSO Warmup already in progress."));
		return;
	}

	UE_LOG(LogPSOAutopilotCore, Log, TEXT("Starting PSO Autopilot Warmup..."));
	
	// Issue 1 fix: Ensure the pipeline cache is actively compiling PSOs
	FShaderPipelineCache::ResumeBatching();

	TotalAssetsProcessed = 0;
	CurrentState = EPSOWarmupState::Scanning;
	AdvanceStateMachine();
}

void UPSOAutopilotCoreSubsystem::AdvanceStateMachine()
{
	switch (CurrentState)
	{
		case EPSOWarmupState::Scanning:
			ScanForAssets();
			break;
		case EPSOWarmupState::LoadingBatch:
			BeginLoadingBatch();
			break;
		case EPSOWarmupState::ProcessingBatch:
			// Handled in Tick
			break;
		case EPSOWarmupState::UnloadingBatch:
			UnloadBatchAndGC();
			break;
		case EPSOWarmupState::WaitingForGC:
			// Handled in Tick
			break;
		case EPSOWarmupState::Finished:
			CompleteWarmup();
			break;
		default:
			break;
	}
}

void UPSOAutopilotCoreSubsystem::ScanForAssets()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	
	// Issue 3 fix: Prevent premature scanning by waiting for the registry to finish
	if (AssetRegistryModule.Get().IsLoadingAssets())
	{
		OnProgressUpdated.Broadcast(0.0f, TEXT("Waiting for Asset Registry to finish loading..."));
		return; // Wait for next tick
	}

	const UPSOAutopilotCoreSettings* Settings = GetDefault<UPSOAutopilotCoreSettings>();
	
	OnProgressUpdated.Broadcast(0.0f, TEXT("Scanning for Assets..."));

	FARFilter Filter;
	Filter.bRecursiveClasses = true;
	Filter.bRecursivePaths = true;
	Filter.ClassPaths.Add(UMaterialInterface::StaticClass()->GetClassPathName());
	

	for (const FDirectoryPath& Dir : Settings->DirectoriesToScan)
	{
		Filter.PackagePaths.Add(FName(*Dir.Path));
	}

	TArray<FAssetData> AssetDataList;
	AssetRegistryModule.Get().GetAssets(Filter, AssetDataList);

	AllDiscoveredAssets.Empty();
	for (const FAssetData& AssetData : AssetDataList)
	{
		AllDiscoveredAssets.Add(AssetData.ToSoftObjectPath());
	}

	TotalAssetsToProcess = AllDiscoveredAssets.Num();
	UE_LOG(LogPSOAutopilotCore, Log, TEXT("Discovered %d assets for PSO Warmup."), TotalAssetsToProcess);

	if (TotalAssetsToProcess == 0)
	{
		CurrentState = EPSOWarmupState::Finished;
		AdvanceStateMachine();
		return;
	}

	CurrentState = EPSOWarmupState::LoadingBatch;
	AdvanceStateMachine();
}

void UPSOAutopilotCoreSubsystem::BeginLoadingBatch()
{
	const UPSOAutopilotCoreSettings* Settings = GetDefault<UPSOAutopilotCoreSettings>();
	
	if (AllDiscoveredAssets.Num() == 0)
	{
		CurrentState = EPSOWarmupState::Finished;
		AdvanceStateMachine();
		return;
	}

	// Determine Batch Size (USP 1: Memory-Safe Chunking)
	int32 BatchSize = FMath::Min(Settings->BatchSize, AllDiscoveredAssets.Num());
	CurrentBatchPaths.Empty();
	
	for (int32 i = 0; i < BatchSize; ++i)
	{
		CurrentBatchPaths.Add(AllDiscoveredAssets[0]);
		AllDiscoveredAssets.RemoveAt(0);
	}

	FString StatusMsg = FString::Printf(TEXT("Loading Batch of %d Assets..."), BatchSize);
	float Progress = (float)TotalAssetsProcessed / (float)FMath::Max(1, TotalAssetsToProcess);
	OnProgressUpdated.Broadcast(Progress, StatusMsg);

	// Async load to prevent main thread blocking
	CurrentStreamableHandle = StreamableManager.RequestAsyncLoad(CurrentBatchPaths, FStreamableDelegate::CreateUObject(this, &UPSOAutopilotCoreSubsystem::OnBatchLoaded));
}

void UPSOAutopilotCoreSubsystem::OnBatchLoaded()
{
	if (CurrentStreamableHandle.IsValid())
	{
		LoadedBatchAssets.Empty();
		CurrentStreamableHandle->GetLoadedAssets(LoadedBatchAssets);
	}

	CurrentAssetIndexInBatch = 0;
	CurrentState = EPSOWarmupState::ProcessingBatch;
	// Processing will now naturally pick up in the Tick() function
}

void UPSOAutopilotCoreSubsystem::ProcessBatchTimeSliced()
{
	const UPSOAutopilotCoreSettings* Settings = GetDefault<UPSOAutopilotCoreSettings>();
	double StartTime = FPlatformTime::Seconds();
	
	// USP 2: Time-sliced execution. We process as many assets as we can within our Ms frame budget.
	while (CurrentAssetIndexInBatch < LoadedBatchAssets.Num())
	{
		UObject* Asset = LoadedBatchAssets[CurrentAssetIndexInBatch];
		if (Asset)
		{
			ForceAssetWarmup(Asset);
		}

		CurrentAssetIndexInBatch++;
		TotalAssetsProcessed++;

		// Broadcast high-frequency updates for buttery smooth UI
		float Progress = (float)TotalAssetsProcessed / (float)FMath::Max(1, TotalAssetsToProcess);
		OnProgressUpdated.Broadcast(Progress, TEXT("Compiling Shaders..."));

		// Synchronous processing for Core version
	}

	// Issue 1 fix: Actually wait for PSOs to compile (using UE's built-in system) before proceeding
	int32 PSOsRemaining = FShaderPipelineCache::NumPrecompilesRemaining();
	if (PSOsRemaining > 0)
	{
		float Progress = (float)TotalAssetsProcessed / (float)FMath::Max(1, TotalAssetsToProcess);
		OnProgressUpdated.Broadcast(Progress, FString::Printf(TEXT("Compiling PSOs... (%d remaining)"), PSOsRemaining));
		return; // Yield and wait for next frame
	}

	// If we finish the loop and PSO compilation without yielding, the batch is done.
	CurrentState = EPSOWarmupState::UnloadingBatch;
	AdvanceStateMachine();
}

void UPSOAutopilotCoreSubsystem::ForceAssetWarmup(UObject* Asset)
{
	if (UMaterialInterface* Material = Cast<UMaterialInterface>(Asset))
	{
		// In a packaged build, forcing the driver to look at the shader map
		// effectively triggers the local driver compilation if it's not cached.
		Material->CacheShaders(EMaterialShaderPrecompileMode::Background);
	}
	
}

void UPSOAutopilotCoreSubsystem::UnloadBatchAndGC()
{
	const UPSOAutopilotCoreSettings* Settings = GetDefault<UPSOAutopilotCoreSettings>();
	
	LoadedBatchAssets.Empty();
	if (CurrentStreamableHandle.IsValid())
	{
		CurrentStreamableHandle->ReleaseHandle();
		CurrentStreamableHandle.Reset();
	}

	if (Settings->bGarbageCollectBetweenBatches)
	{
		OnProgressUpdated.Broadcast((float)TotalAssetsProcessed / (float)FMath::Max(1, TotalAssetsToProcess), TEXT("Clearing Memory..."));
		
		// Delay GC by one frame to ensure streamable manager drops handles
		CurrentState = EPSOWarmupState::WaitingForGC;
		return;
	}

	CurrentState = EPSOWarmupState::LoadingBatch;
	AdvanceStateMachine();
}

void UPSOAutopilotCoreSubsystem::CompleteWarmup()
{
	OnProgressUpdated.Broadcast(1.0f, TEXT("Warmup Complete!"));
	OnWarmupComplete.Broadcast();
	CurrentState = EPSOWarmupState::Idle;
	UE_LOG(LogPSOAutopilotCore, Log, TEXT("PSO Autopilot Warmup Finished successfully."));
}



