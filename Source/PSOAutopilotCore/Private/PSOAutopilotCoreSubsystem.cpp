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
	LatestStatusMessage = TEXT("Ready");
	UE_LOG(LogPSOAutopilotCore, Log, TEXT("PSO Autopilot Subsystem Initialized."));
}

void UPSOAutopilotCoreSubsystem::Deinitialize()
{
	if (CurrentStreamableHandle.IsValid() && CurrentStreamableHandle->IsActive())
	{
		CurrentStreamableHandle->CancelHandle();
	}
	CurrentStreamableHandle.Reset();

	// Drop the borrowed pointers with the handle that was keeping them alive, and leave the state
	// machine idle so a torn-down subsystem is never left mid-warmup.
	LoadedBatchAssets.Reset();
	AllDiscoveredAssets.Reset();
	CurrentBatchPaths.Reset();
	TotalAssetsToProcess = 0;
	TotalAssetsProcessed = 0;
	CurrentAssetIndexInBatch = 0;
	PipelineWaitStartSeconds = -1.0;
	LatestStatusMessage = TEXT("Deinitialized");
	CurrentState = EPSOWarmupState::Idle;

	Super::Deinitialize();
}

void UPSOAutopilotCoreSubsystem::Tick(float DeltaTime)
{
	if (CurrentState == EPSOWarmupState::Scanning)
	{
		ScanForAssets();
	}
	else if (CurrentState == EPSOWarmupState::ProcessingBatch)
	{
		ProcessLoadedBatch();
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

bool UPSOAutopilotCoreSubsystem::IsWarmingUp() const
{
	return CurrentState != EPSOWarmupState::Idle && CurrentState != EPSOWarmupState::Finished;
}

float UPSOAutopilotCoreSubsystem::GetOverallProgress() const
{
	if (CurrentState == EPSOWarmupState::Finished)
	{
		return 1.0f;
	}
	if (TotalAssetsToProcess <= 0)
	{
		return 0.0f;
	}
	return FMath::Clamp((float)TotalAssetsProcessed / (float)TotalAssetsToProcess, 0.0f, 1.0f);
}

void UPSOAutopilotCoreSubsystem::StartWarmup()
{
	if (CurrentState != EPSOWarmupState::Idle && CurrentState != EPSOWarmupState::Finished)
	{
		UE_LOG(LogPSOAutopilotCore, Warning, TEXT("PSO Warmup already in progress."));
		return;
	}

	UE_LOG(LogPSOAutopilotCore, Log, TEXT("Starting PSO Autopilot Warmup..."));

	// Make sure the engine's pipeline cache is actively compiling PSOs while we work.
	FShaderPipelineCache::ResumeBatching();

	TotalAssetsProcessed = 0;
	CurrentAssetIndexInBatch = 0;
	PipelineWaitStartSeconds = -1.0;
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
	
	// Scanning before the registry has finished loading silently returns a partial asset list.
	if (AssetRegistryModule.Get().IsLoadingAssets())
	{
		BroadcastProgress(TEXT("Waiting for Asset Registry to finish loading..."));
		return; // Wait for next tick
	}

	const UPSOAutopilotCoreSettings* Settings = GetDefault<UPSOAutopilotCoreSettings>();
	
	BroadcastProgress(TEXT("Scanning for Assets..."));

	FARFilter Filter;
	Filter.bRecursiveClasses = true;
	Filter.bRecursivePaths = true;
	Filter.ClassPaths.Add(UMaterialInterface::StaticClass()->GetClassPathName());

	for (const FDirectoryPath& Dir : Settings->DirectoriesToScan)
	{
		FString SanitizedPath = Dir.Path.TrimStartAndEnd();
		if (!SanitizedPath.IsEmpty())
		{
			if (SanitizedPath.EndsWith(TEXT("/")))
			{
				SanitizedPath.LeftChopInline(1);
			}
			Filter.PackagePaths.Add(FName(*SanitizedPath));
		}
	}

	// Default to /Game if no valid path was specified
	if (Filter.PackagePaths.Num() == 0)
	{
		Filter.PackagePaths.Add(FName(TEXT("/Game")));
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

	// Memory-safe chunking: take the next slice off the front in one move. Removing element 0 in a
	// loop shifts the whole remaining array each time, which is quadratic in the asset count and
	// very visible when scanning a large /Game.
	const int32 BatchSize = FMath::Min(Settings->BatchSize, AllDiscoveredAssets.Num());
	CurrentBatchPaths.Reset(BatchSize);
	CurrentBatchPaths.Append(AllDiscoveredAssets.GetData(), BatchSize);
	AllDiscoveredAssets.RemoveAt(0, BatchSize);

	BroadcastProgress(FString::Printf(TEXT("Loading Batch of %d Assets..."), BatchSize));

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

void UPSOAutopilotCoreSubsystem::ProcessLoadedBatch()
{
	// Core warms a whole batch in one pass and yields *between* batches. Intra-frame time-slicing
	// of the game thread is a Pro feature and is deliberately not implemented here -- see the
	// feature comparison in README.md. Expect a visible hitch while a batch is warmed; memory
	// safety, not frame pacing, is what this edition guarantees.
	if (CurrentAssetIndexInBatch < LoadedBatchAssets.Num())
	{
		for (; CurrentAssetIndexInBatch < LoadedBatchAssets.Num(); ++CurrentAssetIndexInBatch)
		{
			if (UObject* Asset = LoadedBatchAssets[CurrentAssetIndexInBatch])
			{
				ForceAssetWarmup(Asset);
			}

			TotalAssetsProcessed++;
		}

		// One update per batch, not per asset: this delegate is a dynamic multicast into Blueprint,
		// and firing it hundreds of times in a single frame costs more than the work it reports on.
		// Only the last value of the frame is ever seen anyway.
		BroadcastProgress(TEXT("Compiling Shaders..."));
	}

	// Let the engine's own pipeline cache drain before releasing the batch, but never wait forever:
	// if the queue stops draining, a loading screen gated on OnWarmupComplete would hang for good.
	const int32 PSOsRemaining = FShaderPipelineCache::NumPrecompilesRemaining();
	if (PSOsRemaining > 0)
	{
		if (PipelineWaitStartSeconds < 0.0)
		{
			PipelineWaitStartSeconds = FPlatformTime::Seconds();
		}

		const double WaitedSeconds = FPlatformTime::Seconds() - PipelineWaitStartSeconds;
		if (WaitedSeconds < MaxPipelineWaitSeconds)
		{
			BroadcastProgress(FString::Printf(TEXT("Compiling PSOs... (%d remaining)"), PSOsRemaining));
			return; // Yield and re-check next frame.
		}

		UE_LOG(LogPSOAutopilotCore, Warning,
			TEXT("Gave up after %.1fs waiting for the PSO queue to drain (%d remaining). Continuing so warmup cannot stall."),
			WaitedSeconds, PSOsRemaining);
	}

	PipelineWaitStartSeconds = -1.0;
	CurrentState = EPSOWarmupState::UnloadingBatch;
	AdvanceStateMachine();
}

void UPSOAutopilotCoreSubsystem::BroadcastProgress(const FString& StatusMessage)
{
	LatestStatusMessage = StatusMessage;
	const float Progress = GetOverallProgress();
	OnProgressUpdated.Broadcast(Progress, StatusMessage);
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
		BroadcastProgress(TEXT("Clearing Memory..."));

		// Delay GC by one frame to ensure streamable manager drops handles
		CurrentState = EPSOWarmupState::WaitingForGC;
		return;
	}

	CurrentState = EPSOWarmupState::LoadingBatch;
	AdvanceStateMachine();
}

void UPSOAutopilotCoreSubsystem::CompleteWarmup()
{
	BroadcastProgress(TEXT("Warmup Complete!"));
	OnWarmupComplete.Broadcast();
	CurrentState = EPSOWarmupState::Idle;
	UE_LOG(LogPSOAutopilotCore, Log, TEXT("PSO Autopilot Warmup Finished successfully."));
}
