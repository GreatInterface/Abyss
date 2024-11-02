// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AbyssActivatableWidget.h"

UAbyssActivatableWidget::UAbyssActivatableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TOptional<FUIInputConfig> UAbyssActivatableWidget::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
	case EAbyssWidgetInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
	case EAbyssWidgetInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
	case EAbyssWidgetInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);

	case EAbyssWidgetInputMode::Default:
	default:
		return TOptional<FUIInputConfig>();
	}
}
