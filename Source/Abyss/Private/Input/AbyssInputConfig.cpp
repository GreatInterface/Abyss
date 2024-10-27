// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AbyssInputConfig.h"

#include "Abyss/AbyssLogChannels.h"

UAbyssInputConfig::UAbyssInputConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UInputAction* UAbyssInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag,
	bool bLogNotFound) const
{
	for(const FAbyssInputAction& AbyssIA : NativeInputActions)
	{
		if(AbyssIA.InputAction && (AbyssIA.InputTag == InputTag))
		{
			return AbyssIA.InputAction;
		}
	}

	if(bLogNotFound)
	{
		UE_LOG(LogAbyss, Error, TEXT("Can't Find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UAbyssInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag,
	bool bLogNotFound) const
{
	for(const FAbyssInputAction& IA : AbilityInputActions)
	{
		if(IA.InputAction && (IA.InputTag == InputTag))
		{
			return IA.InputAction;
		}
	}

	if(bLogNotFound)
	{
		UE_LOG(LogAbyss, Error, TEXT("Can't Find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
