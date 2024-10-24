// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/System/AbyssGameplayCueManager.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueSet.h"
#include "GameplayTagsManager.h"
#include "Abyss/AbyssLogChannels.h"

enum class EAbyssEditorLoadMode
{
	// 提前加载所有Cues；在编辑器中加载速度较快，但 PIE 时间较短，而且特效永不失效
	LoadUpfront,

	PreloadAsCuesAreReferenced_GameOnly,

	//异步加载当Cue Tag开始注册
	PreloadAsCuesAreReferenced
};

namespace AbyssGameplayCueManagerCvars
{
	static FAutoConsoleCommand CVarDumpGameplayCues(
		TEXT("Abyss.DumpGameplayCues"),
		TEXT("Shows all assets that were loaded via LyraGameplayCueManager and are currently in memory."),
		FConsoleCommandWithArgsDelegate::CreateStatic(UAbyssGameplayCueManager::DumpGameplayCues));

	static EAbyssEditorLoadMode LoadMode = EAbyssEditorLoadMode::LoadUpfront;	
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
	UAbyssGameplayCueManager* GCM = Get();
	if (!GCM)
	{
		UE_LOG(LogAbyss, Error, TEXT("[AbyssGameCueManager]: DumpGameplayCues failed to found GCM."));
		return;
	}

	const bool bIncludeRefs = Args.Contains(TEXT("Refs"));

	UE_LOG(LogAbyss, Log, TEXT("==== Dumping Always Loaded Gameplay Cue Notifies ===="))
	for (UClass* CueClass : GCM->AlwaysLoadedCues)
	{
		UE_LOG(LogAbyss, Log, TEXT("    %s"), *GetNameSafe(CueClass));
	}
	
	UE_LOG(LogAbyss, Log, TEXT("==== Dumping PreLoaded Gameplay Cue Notifies ===="))
	for (UClass* CueClass : GCM->PreloadedCues)
	{
		TSet<FObjectKey>* ReferencerSet = GCM->PreloadedCueReferencerSet.Find(CueClass);
		int32 NumRefs = ReferencerSet ? ReferencerSet->Num() : 0;
		UE_LOG(LogAbyss, Log, TEXT("	%s (%d refs)"), *GetNameSafe(CueClass), NumRefs);
		if (bIncludeRefs && ReferencerSet)
		{
			for (const FObjectKey& Ref : *ReferencerSet)
			{
				UObject* RefObj = Ref.ResolveObjectPtr();
				UE_LOG(LogAbyss, Log, TEXT("	^- %s"), *GetNameSafe(RefObj));
			}
		}
	}

	UE_LOG(LogAbyss, Log, TEXT("==== Dumping Gameplay Cue Notifies loaded on demand ===="))
	int32 NumMissingCuesLoaded = 0;
	if (UGameplayCueSet* CueSet = GCM->RuntimeGameplayCueObjectLibrary.CueSet)
	{
		for (const FGameplayCueNotifyData& CueData : CueSet->GameplayCueData)
		{
			if (CueData.LoadedGameplayCueClass && !GCM->AlwaysLoadedCues.Contains(CueData.LoadedGameplayCueClass) &&
				!GCM->PreloadedCues.Contains(CueData.LoadedGameplayCueClass))
			{
				++NumMissingCuesLoaded;
				UE_LOG(LogAbyss, Log, TEXT("	%s"), *CueData.LoadedGameplayCueClass.GetPathName());
			}
		}
	}

	UE_LOG(LogAbyss, Log, TEXT("==== Gameplay Cue Notify Summary ===="))
	UE_LOG(LogAbyss, Log, TEXT(" ... %d cues in always loaded list"), GCM->AlwaysLoadedCues.Num());
	UE_LOG(LogAbyss, Log, TEXT(" ... %d cues in preloaded list"), GCM->PreloadedCues.Num());
	UE_LOG(LogAbyss, Log, TEXT(" ... %d cues loaded on demand"), NumMissingCuesLoaded);
	UE_LOG(LogAbyss, Log, TEXT(" ... %d cues in total"), GCM->AlwaysLoadedCues.Num() + GCM->PreloadedCues.Num() + NumMissingCuesLoaded);
}

void UAbyssGameplayCueManager::UpdateDelayLoadDelegateListeners()
{
	UGameplayTagsManager::Get().OnGameplayTagLoadedDelegate.RemoveAll(this);
	FCoreUObjectDelegates::GetPostGarbageCollect().RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	
}
