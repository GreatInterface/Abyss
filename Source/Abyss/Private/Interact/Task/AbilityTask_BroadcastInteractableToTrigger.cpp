// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/Task/AbilityTask_BroadcastInteractableToTrigger.h"
#include "AbilitySystemComponent.h"
#include "Interact/AbyssInteractable.h"
#include "Interact/Ability/AbyssGameplayAbility_Interact.h"

UAbilityTask_BroadcastInteractableToTrigger::UAbilityTask_BroadcastInteractableToTrigger(
	const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
{
}

void UAbilityTask_BroadcastInteractableToTrigger::Activate()
{
	if (WaitTriggers.Num() > 0)
	{
		for (auto Target : WaitTriggers)
		{
			FGameplayEventData Payload;
			Payload.Instigator = Instigator;
			Payload.Target = Target.Get();
			Payload.EventTag = TAG_Ability_Interaction_Activate;
			
			AbilitySystemComponent->HandleGameplayEvent(TAG_Ability_Interaction_Activate, &Payload);
		}
	}

	OnTrigger.Broadcast();
	
	EndTask();
}

UAbilityTask_BroadcastInteractableToTrigger* UAbilityTask_BroadcastInteractableToTrigger::
BroadcastInteractableToTrigger(UGameplayAbility* OwningAbility, AActor* InInstigator,
	const TArray<TSoftObjectPtr<AAbyssInteractable>>& Targets)
{
	UAbilityTask_BroadcastInteractableToTrigger* NewObj = NewAbilityTask<UAbilityTask_BroadcastInteractableToTrigger>(OwningAbility);
	NewObj->Instigator = InInstigator;
	NewObj->WaitTriggers = Targets;

	return NewObj;
}