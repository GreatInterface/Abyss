﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatures/AbyssGameFeaturePolicy.h"

#include "AbilitySystemGlobals.h"
#include "GameFeatureAction.h"
#include "GameFeatureData.h"
#include "GameplayCueSet.h"
#include "AbilitySystem/System/AbyssGameplayCueManager.h"
#include "GameFeatures/GameFeatureAction_AddGameplayCuePath.h"

UAbyssGameFeaturePolicy& UAbyssGameFeaturePolicy::Get()
{
	return UGameFeaturesSubsystem::Get().GetPolicy<UAbyssGameFeaturePolicy>();
}

UAbyssGameFeaturePolicy::UAbyssGameFeaturePolicy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAbyssGameFeaturePolicy::InitGameFeatureManager()
{
	Observes.Add(NewObject<UAbyssGameFeatures_AddGameplayCuePaths>());

	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
	for (UObject* Obj : Observes)
	{
		Subsystem.AddObserver(Obj);
	}
	
	Super::InitGameFeatureManager();
}

void UAbyssGameFeaturePolicy::ShutdownGameFeatureManager()
{
	Super::ShutdownGameFeatureManager();

	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
	for (UObject* Obj : Observes)
	{
		Subsystem.RemoveObserver(Obj);
	}
	Observes.Empty();
}

void UAbyssGameFeatures_AddGameplayCuePaths::OnGameFeatureRegistering(const UGameFeatureData* GameFeatureData,
                                                                      const FString& PluginName, const FString& PluginURL)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UAbyssGameFeatures_AddGameplayCuePaths::OnGameFeatureRegistering);

	const FString PluginRootPath = TEXT("/") + PluginName;
	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
	{
		if (const UGameFeatureAction_AddGameplayCuePath* GFA_AddGameplayCue = Cast<UGameFeatureAction_AddGameplayCuePath>(Action))
		{
			const TArray<FDirectoryPath>& DirsToAdd = GFA_AddGameplayCue->GetDirectoryPathsToAdd();

			if (UAbyssGameplayCueManager* GCM = UAbyssGameplayCueManager::Get())
			{
				UGameplayCueSet* CS = GCM->GetRuntimeCueSet();
				const int32 PreInitializeNumClass = CS ? CS->GameplayCueData.Num() : 0;

				for (const FDirectoryPath& Directory : DirsToAdd)
				{
					FString MutablePath = Directory.Path;
					UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);
					GCM->AddGameplayCueNotifyPath(MutablePath, false);
				}

				if (!DirsToAdd.IsEmpty())
				{
					GCM->InitializeRuntimeObjectLibrary();
				}

				const int32 PostInitializeNumClass = CS ? CS->GameplayCueData.Num() : 0;
				if (PreInitializeNumClass != PostInitializeNumClass)
				{
					GCM->RefreshGameplayCuePrimaryAsset();
				}
			}
		}
	}
}

void UAbyssGameFeatures_AddGameplayCuePaths::OnGameFeatureUnregistering(const UGameFeatureData* GameFeatureData,
	const FString& PluginName, const FString& PluginURL)
{
	const FString PluginRootPath(TEXT("/") + PluginName);

	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
	{
		if (const UGameFeatureAction_AddGameplayCuePath* GFA_AddGameplayCue = Cast<UGameFeatureAction_AddGameplayCuePath>(Action))
		{
			const TArray<FDirectoryPath>& DirsToAdd = GFA_AddGameplayCue->GetDirectoryPathsToAdd();

			if (UGameplayCueManager* GCM = UAbilitySystemGlobals::Get().GetGameplayCueManager())
			{
				int32 NumRemoved = 0;
				for (const FDirectoryPath& Directory : DirsToAdd)
				{
					FString MutablePath = Directory.Path;
					UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);
					NumRemoved += GCM->RemoveGameplayCueNotifyPath(MutablePath, false);
				}

				ensure(NumRemoved == DirsToAdd.Num());

				if (NumRemoved > 0)
				{
					GCM->InitializeRuntimeObjectLibrary();
				}
			}
		}
	}
}
