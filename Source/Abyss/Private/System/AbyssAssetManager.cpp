// Fill out your copyright notice in the Description page of Project Settings.


#include "System/AbyssAssetManager.h"

UAbyssAssetManager::UAbyssAssetManager()
{
}

UAbyssAssetManager& UAbyssAssetManager::Get()
{
	check(GEngine)

	if(UAbyssAssetManager* Singleton = Cast<UAbyssAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	return *NewObject<UAbyssAssetManager>();
}

UObject* UAbyssAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if(AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if(ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if(UAssetManager::IsInitialized())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		return AssetPath.TryLoad();
	}

	return nullptr;
}

bool UAbyssAssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LoadAssetLoads"));
	return bLogAssetLoads;
}

void UAbyssAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if(ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}
