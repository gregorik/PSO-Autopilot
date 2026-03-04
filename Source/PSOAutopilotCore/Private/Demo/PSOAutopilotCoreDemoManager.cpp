// Copyright (c) 2026 GregOrigin. All Rights Reserved.

#include "Demo/PSOAutopilotCoreDemoManager.h"
#include "UI/PSOAutopilotCoreDemoWidget.h"
#include "PSOAutopilotCoreSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameInstance.h"

APSOAutopilotCoreDemoManager::APSOAutopilotCoreDemoManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APSOAutopilotCoreDemoManager::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (!GameInstance) return;

	UPSOAutopilotCoreSubsystem* PSOSubsystem = GameInstance->GetSubsystem<UPSOAutopilotCoreSubsystem>();
	if (!PSOSubsystem) return;

	// Spawn UI
	if (HUDClass)
	{
		ActiveHUD = Cast<UPSOAutopilotCoreDemoWidget>(CreateWidget(GetWorld(), HUDClass));
		if (ActiveHUD)
		{
			ActiveHUD->AddToViewport();
			ActiveHUD->InitializeDemo(PSOSubsystem);
		}
	}

	// Kick off the Torture Test
	PSOSubsystem->StartWarmup();
}
