// Fill out your copyright notice in the Description page of Project Settings.


#include "System/AbyssGameInstance.h"

#include "Abyss/AbyssGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Player/AbyssPlayerController.h"

UAbyssGameInstance::UAbyssGameInstance(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

AAbyssPlayerController* UAbyssGameInstance::GetPrimaryPlayerController() const
{
	return Cast<AAbyssPlayerController>(Super::GetPrimaryPlayerController(false));
}

void UAbyssGameInstance::Init()
{
	Super::Init();

	UGameFrameworkComponentManager* GFCompManager = GetSubsystem<UGameFrameworkComponentManager>(this);

	if(ensure(GFCompManager))
	{
		GFCompManager->RegisterInitState(AbyssGameplayTags::InitState_Spawned, false, FGameplayTag());
		GFCompManager->RegisterInitState(AbyssGameplayTags::InitState_DataAvailable, false, AbyssGameplayTags::InitState_Spawned);
		GFCompManager->RegisterInitState(AbyssGameplayTags::InitState_DataInitialized, false, AbyssGameplayTags::InitState_DataAvailable);
		GFCompManager->RegisterInitState(AbyssGameplayTags::InitState_GameplayReady, false, AbyssGameplayTags::InitState_DataInitialized);
	}
}
