// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "AbyssGameplayCueManager.generated.h"


/**
 * 默认情况下 GameplayCueManager 会扫描我们指定的文件夹寻找 GameplayCueNotifies
 * 并在**开始游戏**时将这些 Notifies 加载到内存中，但这样会导致
 * Notifies以及所引用的包括但不限于声音、粒子都会在游戏开始时被加载到内存，无论是否会使用
 *
 * 所以我们自定义GameplayCueManager子类并重写ShouldAsyncLoadRuntimeObjectLibraries()
 */
UCLASS()
class ABYSS_API UAbyssGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()

public:

	UAbyssGameplayCueManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UAbyssGameplayCueManager* Get();

	//~UGameplayCueManager
	virtual void OnCreated() override;
	virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const override;
	virtual bool ShouldSyncLoadMissingGameplayCues() const override;
	virtual bool ShouldAsyncLoadMissingGameplayCues() const override;
	//~End of UGameplayCueManager
	
	/** 显示所有通过AbyssGameplayCueManager注册并且当前正在被加载到内存的资产 */
	static void DumpGameplayCues(const TArray<FString>& Args);

	void RefreshGameplayCuePrimaryAsset();

private:
	/** @see OnCreated() */
	void UpdateDelayLoadDelegateListeners();

	//~ @see UpdateDelayLoadDelegateListeners()
	void OnGameplayLoaded(const FGameplayTag& GameplayTag);
	void HandlePostGarbageCollect();
	void HandlePostLoadMap(UWorld* World);
	//~End of UpdateDelayLoadDelegateListeners()

	void ProcessLoadedTags();
	void ProcessTagToPreLoad(FGameplayTag Tag, UObject* OwningObject);
	void OnPreloadCueComplete(FSoftObjectPath Path, TWeakObjectPtr<UObject> OwningObject, bool bAlwaysLoadedCue);
	void RegisterPreloadedCue(UClass* LoadedGameplayCueClass, UObject* OwningObject);

	bool ShouldDelayLoadGameplayCues() const;

private:
	struct FLoadedGameplayTagToProcessData
	{
		FGameplayTag Tag;
		TWeakObjectPtr<UObject> WeakOwner;

		FLoadedGameplayTagToProcessData() = default;
		FLoadedGameplayTagToProcessData(const FGameplayTag& InTag, const TWeakObjectPtr<UObject>& InWeakOwner)
			: Tag(InTag)
			, WeakOwner(InWeakOwner)
		{}
	};

private:
	UPROPERTY(Transient)
	TSet<TObjectPtr<UClass>> PreloadedCues;
	TMap<FObjectKey, TSet<FObjectKey>> PreloadedCueReferencerSet;
	
	UPROPERTY(Transient)
	TSet<TObjectPtr<UClass>> AlwaysLoadedCues;

	TArray<FLoadedGameplayTagToProcessData> LoadedGameplayTagToProcess;
	FCriticalSection LoadedGameplayTagToProcess_CS;
	bool bProcessLoadedTagsAfterGC = false;
};
