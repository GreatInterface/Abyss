// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Framework/AbyssGameViewportClient.h"
#include "CommonUISettings.h"
#include "ICommonUIModule.h"

namespace GameViewportTags
{
	UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_Input_HardwareCursor, "Platform.Trait.Input.HardwareCursor")
}

UAbyssGameViewportClient::UAbyssGameViewportClient(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAbyssGameViewportClient::Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance,
                                    bool bCreateNewAudioDevice)
{
	Super::Init(WorldContext, OwningGameInstance, bCreateNewAudioDevice);

	const bool UseHardwareCursor =
		ICommonUIModule::GetSettings().GetPlatformTraits().HasTag(GameViewportTags::TAG_Platform_Trait_Input_HardwareCursor);

	SetUseSoftwareCursorWidgets(!UseHardwareCursor);
}
