// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/AbyssLoadingScreenSubsystem.h"

#include "Blueprint/UserWidget.h"

UAbyssLoadingScreenSubsystem::UAbyssLoadingScreenSubsystem()
{
}

void UAbyssLoadingScreenSubsystem::SetLoadingScreenContextWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (LoadingScreenWidgetClass != NewWidgetClass)
	{
		LoadingScreenWidgetClass = NewWidgetClass;

		OnLoadingScreenWidgetChanged.Broadcast(LoadingScreenWidgetClass);
	}
}

TSubclassOf<UUserWidget> UAbyssLoadingScreenSubsystem::GetLoadingScreenContextWidget()
{
	return LoadingScreenWidgetClass;
}


