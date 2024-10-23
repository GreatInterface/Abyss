// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "AbyssExperienceManagerComponent.generated.h"


namespace UE::GameFeatures { struct FResult; }

class UAbyssExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbyssExperienceLoaded,const UAbyssExperienceDefinition* /*Experience*/)

enum class EAbyssExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeature,
	LoadingChaosTestingDelay,
	ExecutingActions,
	Loaded,
	Deactivating
};

UCLASS()
class ABYSS_API UAbyssExperienceManagerComponent final: public UGameStateComponent
{
	GENERATED_BODY()

public:

	UAbyssExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	//Experience Load 管线第一步
	//@See StartExperienceLoad
	void SetCurrentExperience(FPrimaryAssetId ExperienceId);

	void CallOrRegister_OnExperienceLoaded(FOnAbyssExperienceLoaded::FDelegate&& Delegate);

	const UAbyssExperienceDefinition* GetCurrentExperienceChecked() const;
	
	// Return ture if the experience is full loaded
	bool IsExperienceLoaded() const;

private:

	//Experience Load 管线
	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadComplete();

	void OnActionDeactivationCompleted();
	void OnAllActionsDeactivation();
private:

	UPROPERTY()
	TObjectPtr<const UAbyssExperienceDefinition> CurrentExperience;

	EAbyssExperienceLoadState LoadState = EAbyssExperienceLoadState::Unloaded;

	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginURLs;

	int32 NumObservedPausers = 0;
	int32 NumExpectedPausers = 0;
	
	/**
	 * Delegate called when the experience has finished loading just before others
	 */
	FOnAbyssExperienceLoaded OnExperienceLoaded_HighPriority;

	/** Delegate called when the experience has finished loading */
	FOnAbyssExperienceLoaded OnExperienceLoaded;

	FOnAbyssExperienceLoaded OnExperienceLoaded_LowPriority;
};
