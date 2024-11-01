// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IndicatorDescriptor.h"
#include "Components/ControllerComponent.h"
#include "AbyssIndicatorManagerComponent.generated.h"


class UIndicatorDescriptor;

UCLASS(BlueprintType, Blueprintable)
class ABYSS_API UAbyssIndicatorManagerComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	UAbyssIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer);

	static UAbyssIndicatorManagerComponent* GetComponent(AController* InController);
	const TArray<UIndicatorDescriptor*>& GetIndicators() const { return Indicators; } 

	UFUNCTION(BlueprintCallable, Category = "UI|Indicator")
	void AddIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	UFUNCTION(BlueprintCallable, Category = "UI|Indicator")
	void RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	DECLARE_EVENT_OneParam(UAbyssIndicatorManagerComponent, FIndicatorEvent, UIndicatorDescriptor* InDescriptor)
	FIndicatorEvent OnIndicatorAddedEvent;
	FIndicatorEvent OnIndicatorRemovedEvent;
private:

	UPROPERTY()
	TArray<TObjectPtr<UIndicatorDescriptor>> Indicators;
};

