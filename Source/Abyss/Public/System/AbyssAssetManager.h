// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AbyssAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	
	UAbyssAssetManager();

	static UAbyssAssetManager& Get();

	template<typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPtr, bool bKeepInMemory = true);

private:

	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);
	
	static bool ShouldLogAssetLoads();
	
	void AddLoadedAsset(const UObject* Asset);

private:

	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	FCriticalSection LoadedAssetsCritical;
};

template <typename AssetType>
AssetType* UAbyssAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPtr, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;

	const FSoftObjectPath& AssetPath = AssetPtr.ToSoftObjectPath();

	if(AssetPath.IsValid())
	{
		LoadedAsset = AssetPtr.Get();
		if(!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to Load asset [%s]"), *AssetPtr.ToString());
		}

		if(LoadedAsset && bKeepInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}
