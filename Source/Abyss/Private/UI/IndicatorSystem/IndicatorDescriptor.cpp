// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/IndicatorSystem/IndicatorDescriptor.h"
#include "UI/IndicatorSystem/AbyssIndicatorManagerComponent.h"

void UIndicatorDescriptor::SetManagerComponent(UAbyssIndicatorManagerComponent* InManager)
{
	if(ensure(ManagerPtr.IsExplicitlyNull()))
	{
		ManagerPtr = InManager;
	}
}
