// Fill out your copyright notice in the Description page of Project Settings.


#include "Abyss/Public/GameModes/Experience/AbyssExperienceManagerComponent.h"

#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeaturesSubsystemSettings.h"
#include "Abyss/AbyssLogChannels.h"
#include "Engine/AssetManager.h"
#include "GameModes/Experience/AbyssExperienceActionSet.h"
#include "GameModes/Experience/AbyssExperienceDefinition.h"
#include "System/AbyssAssetManager.h"


UAbyssExperienceManagerComponent::UAbyssExperienceManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}



void UAbyssExperienceManagerComponent::SetCurrentExperience(FPrimaryAssetId ExperienceId)
{
	//通过一个ID (ExperienceId) 查找到该Experience的路径，然后尝试加载它
	UAbyssAssetManager& AssetManager = UAbyssAssetManager::Get();
	FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(ExperienceId);
	TSubclassOf<UAbyssExperienceDefinition> AssetClass = Cast<UClass>(AssetPath.TryLoad());
	check(AssetClass)
	const UAbyssExperienceDefinition* Experience = GetDefault<UAbyssExperienceDefinition>(AssetClass);

	check(Experience)
	check(CurrentExperience == nullptr)
	CurrentExperience = Experience;
	StartExperienceLoad();
}

void UAbyssExperienceManagerComponent::StartExperienceLoad()
{
	check(CurrentExperience)
	check(LoadState == EAbyssExperienceLoadState::Unloaded)

	UE_LOG(LogAbyssExperience, Log, TEXT("EXPERIENCE: StartExperienceLoad(CurrentExperience = %s)"),
	*CurrentExperience->GetPrimaryAssetId().ToString());

	LoadState = EAbyssExperienceLoadState::Loading;

	UAbyssAssetManager& AssetManager = UAbyssAssetManager::Get();

	TSet<FPrimaryAssetId> BundleAssetList;
	TSet<FSoftObjectPath> RawAssetList;

	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());
	for (const TObjectPtr<UAbyssExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if(ActionSet)
		{
			BundleAssetList.Add(ActionSet->GetPrimaryAssetId());
		}
	}
	
	TArray<FName> BundlesToLoad;
	BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
	
	TSharedPtr<FStreamableHandle> BundleLoadHandle = nullptr;
	if (!BundleAssetList.IsEmpty())
	{
		BundleLoadHandle = AssetManager.ChangeBundleStateForPrimaryAssets(BundleAssetList.Array(), BundlesToLoad,{},false,FStreamableDelegate(),FStreamableManager::AsyncLoadHighPriority);
	}

	TSharedPtr<FStreamableHandle> RawLoadHandle = nullptr;
	if(!RawAssetList.IsEmpty())
	{
		RawLoadHandle = AssetManager.LoadAssetList(RawAssetList.Array(),FStreamableDelegate(),FStreamableManager::AsyncLoadHighPriority);
	}

	TSharedPtr<FStreamableHandle> Handle = nullptr;
	if(BundleLoadHandle.IsValid() && RawLoadHandle.IsValid())
	{
		Handle = AssetManager.GetStreamableManager().CreateCombinedHandle({ BundleLoadHandle, RawLoadHandle });
	}
	else
	{
		Handle = BundleLoadHandle.IsValid() ? BundleLoadHandle : RawLoadHandle;
	}

	FStreamableDelegate OnAssetLoadedDelegate = FStreamableDelegate::CreateUObject(this,&ThisClass::OnExperienceLoadComplete);
	if(!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		FStreamableHandle::ExecuteDelegate(OnAssetLoadedDelegate);
	}
	else
	{
		Handle->BindCompleteDelegate(OnAssetLoadedDelegate);

		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetLoadedDelegate]()
		{
			OnAssetLoadedDelegate.ExecuteIfBound();
		}));
	}
}

void UAbyssExperienceManagerComponent::OnExperienceLoadComplete()
{
	check(LoadState == EAbyssExperienceLoadState::Loading)
	check(CurrentExperience != nullptr)

	UE_LOG(LogAbyssExperience, Log, TEXT("EXPERIENCE: OnExperienceLoadComplete(CurrentExperience = %s)"),
	*CurrentExperience->GetPrimaryAssetId().ToString());

	GameFeaturePluginURLs.Reset();

	auto CollectGameFeaturePluginURLs = [This=this](const UPrimaryDataAsset* Context,const TArray<FString>& FeaturePluginList)
	{
		for(const FString& PluginName : FeaturePluginList)
		{
			FString PluginURL;
			if(UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName,/*Out*/ PluginURL))
			{
				This->GameFeaturePluginURLs.Add(PluginURL);
			}
			else
			{
				ensureMsgf(false, TEXT("OnExperienceLoadComplete failed to find plugin URL from PluginName %s for experience %s - fix data, ignoring for this run"), *PluginName, *Context->GetPrimaryAssetId().ToString());
			}
		}
	};

	CollectGameFeaturePluginURLs(CurrentExperience, CurrentExperience->GameFeaturesToEnable);
	for (const TObjectPtr<UAbyssExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if(ActionSet)
		{
			CollectGameFeaturePluginURLs(ActionSet, ActionSet->GameFeatureToEnable);
		}
	}

	NumGameFeaturePluginsLoading = GameFeaturePluginURLs.Num();
	if(NumGameFeaturePluginsLoading > 0)
	{
		LoadState = EAbyssExperienceLoadState::LoadingGameFeature;
		for(const FString& PluginURL : GameFeaturePluginURLs)
		{
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(
				PluginURL,FGameFeaturePluginLoadComplete::CreateUObject(
					this,&ThisClass::OnGameFeaturePluginLoadComplete));
		}
	}
	else
	{
		OnExperienceFullLoadComplete();
	}
}

void UAbyssExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	NumGameFeaturePluginsLoading--;

	if(NumGameFeaturePluginsLoading == 0)
	{
		OnExperienceFullLoadComplete();
	}
}

void UAbyssExperienceManagerComponent::OnExperienceFullLoadComplete()
{
	check(LoadState != EAbyssExperienceLoadState::Loaded)

	LoadState = EAbyssExperienceLoadState::ExecutingActions;

	FGameFeatureActivatingContext Context;

	const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
	if(ExistingWorldContext)
	{
		Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
	}

	auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
	{
		for(UGameFeatureAction* Action : ActionList)
		{
			if(Action)
			{
				Action->OnGameFeatureRegistering();
				Action->OnGameFeatureLoading();
				Action->OnGameFeatureActivating(Context);
			}
		}	
	};

	ActivateListOfActions(CurrentExperience->Actions);
	for(const TObjectPtr<UAbyssExperienceActionSet> ActionSet : CurrentExperience->ActionSets)
	{
		if(ActionSet)
		{
			ActivateListOfActions(ActionSet->Actions);
		}
	}

	LoadState = EAbyssExperienceLoadState::Loaded;

	OnExperienceLoaded_HighPriority.Broadcast(CurrentExperience);
	OnExperienceLoaded_HighPriority.Clear();

	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();

	OnExperienceLoaded_LowPriority.Broadcast(CurrentExperience);
	OnExperienceLoaded_LowPriority.Clear();
}

///////////////////////////////////////////////////////////////////////////////////////////

void UAbyssExperienceManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for(const FString& PluginURL : GameFeaturePluginURLs)
	{
		UGameFeaturesSubsystem::Get().DeactivateGameFeaturePlugin(PluginURL);
	}

	if(LoadState == EAbyssExperienceLoadState::Loaded)
	{
		LoadState = EAbyssExperienceLoadState::Deactivating;

		NumExpectedPausers = INDEX_NONE;
		NumObservedPausers = 0;

		FGameFeatureDeactivatingContext Context(TEXT(""),[this](FStringView){this->OnActionDeactivationCompleted();});

		const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
		if(ExistingWorldContext)
		{
			Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
		}

		{
			auto DeactivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
			{
				for(UGameFeatureAction* Action : ActionList)
				{
					if(Action)
					{
						Action->OnGameFeatureDeactivating(Context);
						Action->OnGameFeatureUnregistering();
					}
				}	
			};

			DeactivateListOfActions(CurrentExperience->Actions);
			for (const TObjectPtr<UAbyssExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
			{
				DeactivateListOfActions(ActionSet->Actions);
			}
		}

		NumExpectedPausers = Context.GetNumPausers();
		if(NumExpectedPausers == NumObservedPausers)
		{
			OnAllActionsDeactivation();
		}
	}
}

bool UAbyssExperienceManagerComponent::ShouldShowLoadingScreen(FString& OutReason) const
{
	if (LoadState != EAbyssExperienceLoadState::Loaded)
	{
		OutReason = TEXT("Experience still Loading");
		return true;
	}

	return false;
}

void UAbyssExperienceManagerComponent::OnActionDeactivationCompleted()
{
	check(IsInGameThread());
	++NumObservedPausers;

	if(NumObservedPausers == NumExpectedPausers)
	{
		OnAllActionsDeactivation();
	}
}

void UAbyssExperienceManagerComponent::OnAllActionsDeactivation()
{
	LoadState = EAbyssExperienceLoadState::Unloaded;
	CurrentExperience = nullptr;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void UAbyssExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnAbyssExperienceLoaded::FDelegate&& Delegate)
{
	if(IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
}

void UAbyssExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_HighPriority(
	FOnAbyssExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		OnExperienceLoaded_HighPriority.Add(MoveTemp(Delegate));
	}
}

const UAbyssExperienceDefinition* UAbyssExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == EAbyssExperienceLoadState::Loaded)
	check(CurrentExperience != nullptr)

	return CurrentExperience;
}

bool UAbyssExperienceManagerComponent::IsExperienceLoaded() const
{
	return (LoadState == EAbyssExperienceLoadState::Loaded) && (CurrentExperience != nullptr); 
}
