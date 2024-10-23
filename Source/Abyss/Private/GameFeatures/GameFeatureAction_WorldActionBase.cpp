// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatures/GameFeatureAction_WorldActionBase.h"

#include "GameFeaturesSubsystem.h"

void UGameFeatureAction_WorldActionBase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	GameInstanceStartHandles.FindOrAdd(Context) = FWorldDelegates::OnStartGameInstance.AddUObject(this,
		&ThisClass::HandleGameInstanceStart,FGameFeatureStateChangeContext(Context));

	//对所有已经存在的WorldContext应用GameFeature
	for(const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		if(Context.ShouldApplyToWorldContext(WorldContext))
		{
			AddToWorld(WorldContext, Context);
		}
	}
}

void UGameFeatureAction_WorldActionBase::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	FDelegateHandle* FoundHandle = GameInstanceStartHandles.Find(Context);
	if(ensure(FoundHandle))
	{
		FWorldDelegates::OnStartGameInstance.Remove(*FoundHandle);
	}
}

void UGameFeatureAction_WorldActionBase::HandleGameInstanceStart(UGameInstance* GameInstance,
	FGameFeatureStateChangeContext ChangeContext)
{
	if(FWorldContext* WorldContext = GameInstance->GetWorldContext())
	{
		if(ChangeContext.ShouldApplyToWorldContext(*WorldContext))
		{
			AddToWorld(*WorldContext,ChangeContext);
		}
	}
}
