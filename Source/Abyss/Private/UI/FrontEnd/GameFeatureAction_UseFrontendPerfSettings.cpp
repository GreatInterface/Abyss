// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FrontEnd/GameFeatureAction_UseFrontendPerfSettings.h"


void UGameFeatureAction_UseFrontendPerfSettings::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	ApplicationCounter++;
	if (ApplicationCounter == 1)
	{
		//TODO : SettingLocal::Get()->SetShouldUseFrontendPerformanceSettings(true)
	}
}

void UGameFeatureAction_UseFrontendPerfSettings::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	ApplicationCounter--;
	check(ApplicationCounter >= 0);

	if (ApplicationCounter == 0)
	{
		//TODO : SettingLocal::Get()->SetShouldUseFrontendPerformanceSettings(false)
	}
}
