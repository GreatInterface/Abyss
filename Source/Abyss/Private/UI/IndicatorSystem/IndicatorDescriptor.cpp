// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/IndicatorSystem/IndicatorDescriptor.h"
#include "UI/IndicatorSystem/AbyssIndicatorManagerComponent.h"

bool FIndicatorProjection::Project(const UIndicatorDescriptor& IndicatorDescriptor,
	const FSceneViewProjectionData& InProjectionData, const FVector2f& ScreenSize, FVector& ScreenPositionWithDepth)
{
	//TODO :
	unimplemented();
}

void UIndicatorDescriptor::SetManagerComponent(UAbyssIndicatorManagerComponent* InManager)
{
	if(ensure(ManagerPtr.IsExplicitlyNull()))
	{
		ManagerPtr = InManager;
	}
}
