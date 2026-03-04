// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PSOAutopilotCoreDemoWidget.generated.h"

class UPSOAutopilotCoreSubsystem;
class UTextBlock;
class UProgressBar;
class UImage;

/**
 * A UI overlay to visualize the PSO Autopilot USPs (Smooth Spinner, Live Telemetry, Timer).
 */
UCLASS()
class PSOAUTOPILOTCORE_API UPSOAutopilotCoreDemoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeDemo(UPSOAutopilotCoreSubsystem* Subsystem);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StatusText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SpinnerImage;

private:
	UFUNCTION()
	void OnProgressUpdated(float OverallProgress, const FString& CurrentStatusMessage);

	UFUNCTION()
	void OnWarmupComplete();

	bool bIsWarmingUp = false;
	float WarmupTimer = 0.0f;
	float SpinnerAngle = 0.0f;
};
