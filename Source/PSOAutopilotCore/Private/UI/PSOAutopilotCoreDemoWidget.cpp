// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#include "UI/PSOAutopilotCoreDemoWidget.h"
#include "PSOAutopilotCoreSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"

void UPSOAutopilotCoreDemoWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPSOAutopilotCoreDemoWidget::InitializeDemo(UPSOAutopilotCoreSubsystem* Subsystem)
{
	if (!Subsystem) return;

	Subsystem->OnProgressUpdated.AddDynamic(this, &UPSOAutopilotCoreDemoWidget::OnProgressUpdated);
	Subsystem->OnWarmupComplete.AddDynamic(this, &UPSOAutopilotCoreDemoWidget::OnWarmupComplete);

	bIsWarmingUp = true;
	WarmupTimer = 0.0f;
	
	if (StatusText)
	{
		StatusText->SetText(FText::FromString(TEXT("Initializing...")));
	}
}

void UPSOAutopilotCoreDemoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// USP 2 Validation: If time-slicing works, this spinner will stay perfectly smooth at 60fps.
	// If it stutters or freezes, the background processing is blocking the Game Thread.
	if (SpinnerImage)
	{
		SpinnerAngle += InDeltaTime * 360.0f; // 1 full rotation per second
		if (SpinnerAngle > 360.0f) SpinnerAngle -= 360.0f;
		SpinnerImage->SetRenderTransformAngle(SpinnerAngle);
	}

	if (bIsWarmingUp)
	{
		WarmupTimer += InDeltaTime;
		if (TimerText)
		{
			TimerText->SetText(FText::FromString(FString::Printf(TEXT("Elapsed Time: %.2fs"), WarmupTimer)));
		}
	}
}

void UPSOAutopilotCoreDemoWidget::OnProgressUpdated(float OverallProgress, const FString& CurrentStatusMessage)
{
	if (StatusText)
	{
		StatusText->SetText(FText::FromString(CurrentStatusMessage));
	}

	if (ProgressBar)
	{
		ProgressBar->SetPercent(OverallProgress);
	}
}

void UPSOAutopilotCoreDemoWidget::OnWarmupComplete()
{
	bIsWarmingUp = false;
	
	if (StatusText)
	{
		StatusText->SetText(FText::FromString(TEXT("Warmup Complete! Ready to play.")));
	}
}
