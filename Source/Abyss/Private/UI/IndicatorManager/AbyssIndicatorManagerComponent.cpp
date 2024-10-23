// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IndicatorSystem/AbyssIndicatorManagerComponent.h"

#include "UI/IndicatorSystem/IndicatorDescriptor.h"


UAbyssIndicatorManagerComponent::UAbyssIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;
	bAutoRegister = true;
}

UAbyssIndicatorManagerComponent* UAbyssIndicatorManagerComponent::GetIndicatorManagerComponent(
	AController* InController)
{
	if(InController)
	{
		return InController->FindComponentByClass<UAbyssIndicatorManagerComponent>();
	}

	return nullptr;
}

void UAbyssIndicatorManagerComponent::AddIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	check(IndicatorDescriptor);
	IndicatorDescriptor->SetManagerComponent(this);
	OnIndicatorAddedEvent.Broadcast(IndicatorDescriptor);
	Indicators.Add(MoveTemp(IndicatorDescriptor));
}

void UAbyssIndicatorManagerComponent::RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	if(IndicatorDescriptor)
	{
		ensure(IndicatorDescriptor->GetManagerComponent() == this);

		OnIndicatorRemovedEvent.Broadcast(IndicatorDescriptor);

		Indicators.Remove(MoveTemp(IndicatorDescriptor));
	}
}
