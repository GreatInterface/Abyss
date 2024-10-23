// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/System/AbyssGameplayCueManager.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagsManager.h"

enum class EAbyssEditorLoadMode
{
	// 提前加载所有Cues；在编辑器中加载速度较快，但 PIE 时间较短，而且特效永不失效
	LoadUpfront,

	PreloadAsCuesAreReferenced_GameOnly,

	PreloadAsCuesAreReferenced
};

namespace AbyssGameplayCueManagerCvars
{
	static FAutoConsoleCommand CVarDumpGameplayCues(
		TEXT("Abyss.DumpGameplayCues"),
		TEXT("Shows all assets that were loaded via LyraGameplayCueManager and are currently in memory."),
		FConsoleCommandWithArgsDelegate::CreateStatic(UAbyssGameplayCueManager::DumpGameplayCues));

	
}

////////////////////////////////////////////////////////////////////////////////////////////

UAbyssGameplayCueManager::UAbyssGameplayCueManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAbyssGameplayCueManager* UAbyssGameplayCueManager::Get()
{
	return Cast<UAbyssGameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager());
}

void UAbyssGameplayCueManager::OnCreated()
{
	Super::OnCreated();
}

bool UAbyssGameplayCueManager::ShouldAsyncLoadRuntimeObjectLibraries() const
{
	return Super::ShouldAsyncLoadRuntimeObjectLibraries();
}

bool UAbyssGameplayCueManager::ShouldSyncLoadMissingGameplayCues() const
{
	return Super::ShouldSyncLoadMissingGameplayCues();
}

bool UAbyssGameplayCueManager::ShouldAsyncLoadMissingGameplayCues() const
{
	return Super::ShouldAsyncLoadMissingGameplayCues();
}

void UAbyssGameplayCueManager::DumpGameplayCues(const TArray<FString>& Args)
{
	
}

void UAbyssGameplayCueManager::UpdateDelayLoadDelegateListeners()
{
	UGameplayTagsManager::Get().OnGameplayTagLoadedDelegate.RemoveAll(this);
	FCoreUObjectDelegates::GetPostGarbageCollect().RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	
}
