// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../IndicatorDescriptor.h"

#include "IndicatorWidgetInterface.generated.h"

class UIndicatorDescriptor;
// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UIndicatorWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ABYSS_API IIndicatorWidgetInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category="Indicator")
	void BindIndicator(UIndicatorDescriptor* Indicator);

	UFUNCTION(BlueprintNativeEvent, Category="Indicator")
	void UnbindIndicator(const UIndicatorDescriptor* Indicator);
};
