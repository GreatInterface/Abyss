// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AbyssInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"



UAbyssInputComponent::UAbyssInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAbyssInputComponent::AddInputMapping(const UAbyssInputConfig* Config,
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	//TODO : 自定义
}

void UAbyssInputComponent::RemoveInputMapping(const UAbyssInputConfig* Config,
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	//TODO: 自定义
}

void UAbyssInputComponent::RemoveAbilityActionsByHandles(TArray<uint32>& BindHandles)
{
	for(uint32& Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
}
