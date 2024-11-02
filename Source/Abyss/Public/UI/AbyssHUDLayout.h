// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbyssActivatableWidget.h"
#include "AbyssHUDLayout.generated.h"

/**
* ULyraHUDLayout :
*	用于布局玩家 HUD 的小部件(通常由Experience中的 “Add Widgets”操作指定)
 */
UCLASS(Abstract, BlueprintType, Blueprintable, meta=(DisplayName = "Abyss HUD Layout", Category="Abyss|HUD"))
class ABYSS_API UAbyssHUDLayout : public UAbyssActivatableWidget
{
	GENERATED_BODY()

public:
	
	UAbyssHUDLayout(const FObjectInitializer& ObjectInitializer);

	//~UserWidget
	void NativeOnInitialized() override;

protected:

	void HandleEscapeAction();
	
	UPROPERTY(EditDefaultsOnly, Category="Abyss|UI")
	TSoftClassPtr<UCommonActivatableWidget> EscapeMenuClass;
};
