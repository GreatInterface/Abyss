// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "AbyssActivatableWidget.generated.h"

UENUM(BlueprintType)
enum class EAbyssWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

// 可激活的UI组件，激活后可自动驱动所需的输入配置
UCLASS(Abstract, Blueprintable)
class ABYSS_API UAbyssActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	UAbyssActivatableWidget(const FObjectInitializer& ObjectInitializer);

	//~UCommonActivatableWidget
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

protected:

	//激活此用户界面时希望使用的输入模式(Game or UI ?)，例如，是否希望按下的键仍能到达游戏/玩家控制器？
	UPROPERTY(EditDefaultsOnly, Category="Abyss|Input")
	EAbyssWidgetInputMode InputConfig = EAbyssWidgetInputMode::Default;

	//当游戏获得输入时，所希望的鼠标行为。
	UPROPERTY(EditDefaultsOnly, Category="Abyss|Input")
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};
