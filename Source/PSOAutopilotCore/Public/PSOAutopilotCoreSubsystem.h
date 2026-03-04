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
 * The core engine subsystem handling Memory-Safe Chunking and Time-Sliced PSO compilation.
 * Destroys the competition by ensuring loading screens never freeze and games never crash from OOM.
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

	/** Fires continuously during warm-up to drive UI loading bars smoothly. */
	UPROPERTY(BlueprintAssignable, Category = "PSO Autopilot")
	FPSOWarmupProgressDelegate OnProgressUpdated;

	/** Fires when the entire warm-up sequence is complete. */
	UPROPERTY(BlueprintAssignable, Category = "PSO Autopilot")
	FPSOWarmupCompleteDelegate OnWarmupComplete;

private:
	EPSOWarmupState CurrentState;
	
	TArray<FSoftObjectPath> AllDiscoveredAssets;
	TArray<FSoftObjectPath> CurrentBatchPaths;
	TArray<UObject*> LoadedBatchAssets;

	int32 TotalAssetsToProcess;
	int32 TotalAssetsProcessed;
	int32 CurrentBatchIndex;
	int32 CurrentAssetIndexInBatch;

	TSharedPtr<FStreamableHandle> CurrentStreamableHandle;
	FStreamableManager StreamableManager;

	void AdvanceStateMachine();
	void ScanForAssets();
	void BeginLoadingBatch();
	void OnBatchLoaded();
	void ProcessBatchTimeSliced();
	void UnloadBatchAndGC();
	void CompleteWarmup();

	void ForceAssetWarmup(UObject* Asset);
};
