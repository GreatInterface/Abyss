// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_SplitscreenConfig.generated.h"


UCLASS(MinimalAPI, meta = (DisplayName = "Splitscreen Config"))
class UGameFeatureAction_SplitscreenConfig : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:

	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

public:

	UPROPERTY(EditAnywhere, Category="Actions")
	bool bDisableSplitscreen = true;

private:
	TArray<FObjectKey> LocalDisableVotes;

	inline static TMap<FObjectKey, int32> GlobalDisableVotes = TMap<FObjectKey, int32>(); 
};
