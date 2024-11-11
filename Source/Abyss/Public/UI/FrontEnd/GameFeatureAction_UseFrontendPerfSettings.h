// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "GameFeatureAction_UseFrontendPerfSettings.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, meta=(DisplayName="Use Frontend Perf Settings"))
class UGameFeatureAction_UseFrontendPerfSettings : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

private:
	inline static int32 ApplicationCounter = 0;
};
