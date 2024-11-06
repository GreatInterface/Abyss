// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/AbyssInteractable.h"
#include "GameplayMessageSubsystem.h"
#include "Interact/Message/AbyssInteractMessageType.h"


AAbyssInteractable::AAbyssInteractable()
{
}

void AAbyssInteractable::GatherInteractionOptions(const FInteractionQuery& InteractionQuery,
	FInteractionOptionBuilder& OptionBuilder)
{
	/**OUT*/ OptionBuilder.AddInteractionOption(InteractOption);
}

void AAbyssInteractable::CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag,
	FGameplayEventData& InOutEventData)
{
}

void AAbyssInteractable::BroadcastMessageForInteractableTargets(
	const TArray<TObjectPtr<AAbyssInteractable>>& InteractableTargets, FGameplayTag Channel,
	FAbyssInteractMessage& MessageStruct)
{
	if (InteractableTargets.IsEmpty())
	{
		return;
	}

	TArray<TScriptInterface<IInteractableTarget>> Interfaces;
	for (AAbyssInteractable* Interactable : InteractableTargets)
	{
		TScriptInterface<AAbyssInteractable> Interface(Interactable);
		if (Interface)
		{
			Interfaces.Add(Interface);
		}
	}

	MessageStruct.Targets = Interfaces;
	
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (UGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<UGameplayMessageSubsystem>())
			{
				MessageSubsystem->BroadcastMessage(Channel, MessageStruct);
			}
		}		
	}
}

