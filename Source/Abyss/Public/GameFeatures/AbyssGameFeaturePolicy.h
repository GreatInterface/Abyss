﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeaturesProjectPolicies.h"
#include "GameFeatureStateChangeObserver.h"
#include "AbyssGameFeaturePolicy.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Config = Game)
class UAbyssGameFeaturePolicy : public UDefaultGameFeaturesProjectPolicies
{
	GENERATED_BODY()

public:

	static UAbyssGameFeaturePolicy& Get();

	UAbyssGameFeaturePolicy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UGameFeatureProjectPolicies interface
	virtual void InitGameFeatureManager() override;
	virtual void ShutdownGameFeatureManager() override;
	//~End of UGameFeatureProjectPolicies interface

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> Observes;
};

UCLASS()
class UAbyssGameFeatures_AddGameplayCuePaths : public UObject, public IGameFeatureStateChangeObserver
{
	GENERATED_BODY()

public:

	virtual void OnGameFeatureRegistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL) override;
	virtual void OnGameFeatureUnregistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL) override;
	
};
