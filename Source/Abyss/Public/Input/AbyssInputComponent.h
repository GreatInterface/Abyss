// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbyssInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AbyssInputComponent.generated.h"


class UEnhancedInputLocalPlayerSubsystem;
class UAbyssInputConfig;

UCLASS(Config = Input)
class ABYSS_API UAbyssInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	UAbyssInputComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void AddInputMapping(const UAbyssInputConfig* Config, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	void RemoveInputMapping(const UAbyssInputConfig* Config,UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;

	template<class UserClass, typename FuncType>
	void BindNativeAction(const UAbyssInputConfig* Config, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UAbyssInputConfig* Config, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	void RemoveAbilityActionsByHandles(TArray<uint32>& BindHandles);
};

template <class UserClass, typename FuncType>
void UAbyssInputComponent::BindNativeAction(const UAbyssInputConfig* Config, const FGameplayTag& InputTag,
	ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	if(const UInputAction* Action = Config->FindNativeInputActionForTag(InputTag,bLogIfNotFound))
	{
		BindAction(Action, TriggerEvent, Object, Func);
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UAbyssInputComponent::BindAbilityActions(const UAbyssInputConfig* Config, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(Config)

	for(const FAbyssInputAction& Action : Config->AbilityInputActions)
	{
		if(Action.InputAction && Action.InputTag.IsValid())
		{
			if(PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}

			if(ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}
