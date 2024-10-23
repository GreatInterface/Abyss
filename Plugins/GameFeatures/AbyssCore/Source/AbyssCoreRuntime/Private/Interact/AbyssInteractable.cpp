// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/AbyssInteractable.h"


AAbyssInteractable::AAbyssInteractable()
{
}

void AAbyssInteractable::GatherInteractionOptions(const FInteractionQuery& InteractionQuery,
	FInteractionOptionBuilder& OptionBuilder)
{
	/**OUT*/ OptionBuilder.AddInteractionOption(Option);
}

void AAbyssInteractable::CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag,
	FGameplayEventData& InOutEventData)
{
}

