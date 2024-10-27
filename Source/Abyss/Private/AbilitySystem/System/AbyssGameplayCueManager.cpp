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

	bool GetEditorLoadModePolicy(const EAbyssEditorLoadMode& InMode)
	{
		switch (InMode)
		{
		case EAbyssEditorLoadMode::LoadUpfront:
			return false;
		case EAbyssEditorLoadMode::PreloadAsCuesAreReferenced_GameOnly:
#if WITH_EDITOR
			if (GIsEditor)
			{
				return false;
			}
#endif
			return true;
		case EAbyssEditorLoadMode::PreloadAsCuesAreReferenced:
			return true;

			default: return false;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

struct FGameplayCueTagThreadSynchronizeGraphTask : public FAsyncGraphTaskBase
{
	FGameplayCueTagThreadSynchronizeGraphTask(TFunction<void()>&& Task)
		: TheTask(MoveTemp(Task))
	{}

	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& GraphEvent)
	{
		TheTask();
	}

	ENamedThreads::Type GetDesiredThread() 
	{
		return ENamedThreads::GameThread;
	}

	TFunction<void()> TheTask;
};

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

	UpdateDelayLoadDelegateListeners();
}

bool UAbyssGameplayCueManager::ShouldAsyncLoadRuntimeObjectLibraries() const
{
	switch (AbyssGameplayCueManagerCvars::LoadMode) {
	case EAbyssEditorLoadMode::LoadUpfront:
		return true;
	case EAbyssEditorLoadMode::PreloadAsCuesAreReferenced_GameOnly:
#if WITH_EDITOR
		if (GIsEditor)
		{
			return false;
		}
#endif
		break;
	case EAbyssEditorLoadMode::PreloadAsCuesAreReferenced:
		break;
	}
	
	return !ShouldDelayLoadGameplayCues();
}

bool UAbyssGameplayCueManager::ShouldSyncLoadMissingGameplayCues() const
{
	return false;
}

bool UAbyssGameplayCueManager::ShouldAsyncLoadMissingGameplayCues() const
{
	return true;
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

	bool bContinue = AbyssGameplayCueManagerCvars::GetEditorLoadModePolicy(AbyssGameplayCueManagerCvars::LoadMode);
	if (!bContinue) return;
	
	UGameplayTagsManager::Get().OnGameplayTagLoadedDelegate.AddUObject(this, &ThisClass::OnGameplayLoaded);
	FCoreUObjectDelegates::GetPostGarbageCollect().AddUObject(this, &ThisClass::HandlePostGarbageCollect);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);
	
}

void UAbyssGameplayCueManager::OnGameplayLoaded(const FGameplayTag& GameplayTag)
{
	FScopeLock ScopeLock(&LoadedGameplayTagToProcess_CS);

	bool bStartTask = LoadedGameplayTagToProcess.Num() == 0;
	
	FUObjectSerializeContext* LoadContext = FUObjectThreadContext::Get().GetSerializeContext();
	UObject* OwningObj = LoadContext ? LoadContext->SerializedObject : nullptr;
	LoadedGameplayTagToProcess.Emplace(GameplayTag, OwningObj);

	if (bStartTask)
	{
		TGraphTask<FGameplayCueTagThreadSynchronizeGraphTask>::CreateTask()
			.ConstructAndDispatchWhenReady([]()
			{
				if (GIsRunning)
				{
					if (UAbyssGameplayCueManager* GCM = Get())
					{
						if (IsGarbageCollecting())
						{
							GCM->bProcessLoadedTagsAfterGC = true;
						}
						else
						{
							GCM->ProcessLoadedTags();
						}
					}
				}
			});
	}
}

void UAbyssGameplayCueManager::ProcessLoadedTags()
{
	TArray<FLoadedGameplayTagToProcessData> TaskLoadedGameplayTagToProcess;
	{
		FScopeLock ScopeLock(&LoadedGameplayTagToProcess_CS);
		TaskLoadedGameplayTagToProcess = MoveTemp(LoadedGameplayTagToProcess);
	}

	if (GIsRunning)
	{
		if (UGameplayCueSet* CueSet = RuntimeGameplayCueObjectLibrary.CueSet)
		{
			for (const auto& LoadedTagData : TaskLoadedGameplayTagToProcess)
			{
				if (CueSet->GameplayCueDataMap.Contains(LoadedTagData.Tag))
				{
					ProcessTagToPreLoad(LoadedTagData.Tag, LoadedTagData.WeakOwner.Get());
				}
			}
		}
	}
}

void UAbyssGameplayCueManager::ProcessTagToPreLoad(FGameplayTag Tag, UObject* OwningObject)
{
	bool bContinue = AbyssGameplayCueManagerCvars::GetEditorLoadModePolicy(AbyssGameplayCueManagerCvars::LoadMode);
	if (!bContinue) return;

	UGameplayCueSet* CueSet = RuntimeGameplayCueObjectLibrary.CueSet;
	check(CueSet != nullptr);

	int32* DataIdx = CueSet->GameplayCueDataMap.Find(Tag);
	if (DataIdx && CueSet->GameplayCueData.IsValidIndex(*DataIdx))
	{
		const FGameplayCueNotifyData& CueData = CueSet->GameplayCueData[ *DataIdx ];

		UClass* LoadedGameplayCueClass = FindObject<UClass>(nullptr, *CueData.GameplayCueNotifyObj.ToString());
		if (LoadedGameplayCueClass)
		{
			RegisterPreloadedCue(LoadedGameplayCueClass, OwningObject);
		}
		else
		{
			bool bAlwaysLoadedCue = OwningObject == nullptr;
			TWeakObjectPtr WeakOwner = OwningObject;
			StreamableManager.RequestAsyncLoad(CueData.GameplayCueNotifyObj,
				FStreamableDelegate::CreateUObject(
					this, &ThisClass::OnPreloadCueComplete, CueData.GameplayCueNotifyObj, WeakOwner, bAlwaysLoadedCue),
				FStreamableManager::DefaultAsyncLoadPriority,false, false,
				TEXT("GameplayCueManager"));
		}
	}
	
}

void UAbyssGameplayCueManager::OnPreloadCueComplete(FSoftObjectPath Path, TWeakObjectPtr<UObject> OwningObject,
	bool bAlwaysLoadedCue)
{
	if (bAlwaysLoadedCue || OwningObject.IsValid())
	{
		if (UClass* LoadedGameplayCueClass = Cast<UClass>(Path.ResolveObject()))
		{
			RegisterPreloadedCue(LoadedGameplayCueClass, OwningObject.Get());
		}
	}
}

void UAbyssGameplayCueManager::RegisterPreloadedCue(UClass* LoadedGameplayCueClass, UObject* OwningObject)
{
	check(LoadedGameplayCueClass)

	const bool bAlwaysLoadedCue = OwningObject == nullptr;
	if (bAlwaysLoadedCue)
	{
		AlwaysLoadedCues.Add(LoadedGameplayCueClass);
		PreloadedCues.Remove(LoadedGameplayCueClass);
		PreloadedCueReferencerSet.Remove(LoadedGameplayCueClass);
	}
	else if ((OwningObject != LoadedGameplayCueClass) &&
			 (OwningObject != LoadedGameplayCueClass->GetDefaultObject()) &&
			 (!AlwaysLoadedCues.Contains(LoadedGameplayCueClass)))
	{
		PreloadedCues.Add(LoadedGameplayCueClass);
		TSet<FObjectKey>& ReferencerSet = PreloadedCueReferencerSet.FindOrAdd(LoadedGameplayCueClass);
		ReferencerSet.Add(OwningObject);
	}
}

bool UAbyssGameplayCueManager::ShouldDelayLoadGameplayCues() const
{
	const bool bClientDelayLoadGameplayCues = true;
	return !IsRunningDedicatedServer() && bClientDelayLoadGameplayCues;
}

void UAbyssGameplayCueManager::HandlePostGarbageCollect()
{
	if (bProcessLoadedTagsAfterGC)
	{
		ProcessLoadedTags();
	}
	bProcessLoadedTagsAfterGC = false;
}

void UAbyssGameplayCueManager::HandlePostLoadMap(UWorld* World)
{
	if (UGameplayCueSet* CueSet = RuntimeGameplayCueObjectLibrary.CueSet)
	{
		for (UClass* CueClass : AlwaysLoadedCues)
		{
			CueSet->RemoveLoadedClass(CueClass);
		}

		for (UClass* CueClass : PreloadedCues)
		{
			CueSet->RemoveLoadedClass(CueClass);
		}
	}

	auto CueIt = PreloadedCues.CreateIterator();
	for (; CueIt; ++CueIt)
	{
		TSet<FObjectKey>& ReferencerSet = PreloadedCueReferencerSet.FindChecked(*CueIt);
		for (auto RefIt = ReferencerSet.CreateIterator(); RefIt; ++RefIt)
		{
			if (!RefIt->ResolveObjectPtr())
			{
				RefIt.RemoveCurrent();
			}
		}
		if (ReferencerSet.IsEmpty())
		{
			PreloadedCueReferencerSet.Remove(*CueIt);
			CueIt.RemoveCurrent();
		}
	}
}