// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Engine/StreamableManager.h"
#include "PSOAutopilotCoreSubsystem.generated.h"

UENUM(BlueprintType)
enum class EPSOWarmupState : uint8
{
	Idle,
	Scanning,
	LoadingBatch,
	ProcessingBatch,
	UnloadingBatch,
	WaitingForGC,
	Finished
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPSOWarmupProgressDelegate, float, OverallProgress, const FString&, CurrentStatusMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPSOWarmupCompleteDelegate);

/**
 * Warms material shaders in memory-safe chunks: assets are streamed in batches, warmed, then
 * released and garbage collected before the next batch, so peak memory stays flat on projects far
 * too large to load at once.
 *
 * Scope note: this edition yields between batches, not within one. Intra-frame time-slicing of the
 * game thread, smart cache skipping, setup wizard, and engine PSO pacing are Pro features -- see README.md.
 */
UCLASS()
class PSOAUTOPILOTCORE_API UPSOAutopilotCoreSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// FTickableGameObject Interface
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;

	/** Starts the highly optimized, asynchronous PSO warm-up process. */
	UFUNCTION(BlueprintCallable, Category = "PSO Autopilot")
	void StartWarmup();

	/** Whether the subsystem is currently executing a warmup cycle. */
	UFUNCTION(BlueprintPure, Category = "PSO Autopilot")
	bool IsWarmingUp() const;

	/** Returns the current state of the warmup state machine. */
	UFUNCTION(BlueprintPure, Category = "PSO Autopilot")
	EPSOWarmupState GetWarmupState() const { return CurrentState; }

	/** Returns overall progress as a normalized float (0.0 to 1.0). */
	UFUNCTION(BlueprintPure, Category = "PSO Autopilot")
	float GetOverallProgress() const;

	/** Returns total number of discovered assets to process in this warmup run. */
	UFUNCTION(BlueprintPure, Category = "PSO Autopilot")
	int32 GetTotalAssetsToProcess() const { return TotalAssetsToProcess; }

	/** Returns number of assets processed so far in this warmup run. */
	UFUNCTION(BlueprintPure, Category = "PSO Autopilot")
	int32 GetTotalAssetsProcessed() const { return TotalAssetsProcessed; }

	/** Returns the latest human-readable status message. */
	UFUNCTION(BlueprintPure, Category = "PSO Autopilot")
	FString GetCurrentStatusMessage() const { return LatestStatusMessage; }

	/** Fires continuously during warm-up to drive UI loading bars smoothly. */
	UPROPERTY(BlueprintAssignable, Category = "PSO Autopilot")
	FPSOWarmupProgressDelegate OnProgressUpdated;

	/** Fires when the entire warm-up sequence is complete. */
	UPROPERTY(BlueprintAssignable, Category = "PSO Autopilot")
	FPSOWarmupCompleteDelegate OnWarmupComplete;

private:
	/** Upper bound on how long a single batch waits for the engine's PSO queue to drain. */
	static constexpr double MaxPipelineWaitSeconds = 30.0;

	EPSOWarmupState CurrentState = EPSOWarmupState::Idle;

	TArray<FSoftObjectPath> AllDiscoveredAssets;
	TArray<FSoftObjectPath> CurrentBatchPaths;

	/**
	 * Raw pointers, kept alive solely by CurrentStreamableHandle. Never hold these without the
	 * handle, and never keep them past UnloadBatchAndGC().
	 */
	TArray<UObject*> LoadedBatchAssets;

	int32 TotalAssetsToProcess = 0;
	int32 TotalAssetsProcessed = 0;
	int32 CurrentAssetIndexInBatch = 0;
	FString LatestStatusMessage;

	/** Timestamp the current batch began waiting on the PSO queue; negative when not waiting. */
	double PipelineWaitStartSeconds = -1.0;

	TSharedPtr<FStreamableHandle> CurrentStreamableHandle;
	FStreamableManager StreamableManager;

	void AdvanceStateMachine();
	void ScanForAssets();
	void BeginLoadingBatch();
	void OnBatchLoaded();
	void ProcessLoadedBatch();
	void UnloadBatchAndGC();
	void CompleteWarmup();

	void ForceAssetWarmup(UObject* Asset);
	void BroadcastProgress(const FString& StatusMessage);
};
