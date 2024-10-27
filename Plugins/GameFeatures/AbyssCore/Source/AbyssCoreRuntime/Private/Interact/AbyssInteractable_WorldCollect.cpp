// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/AbyssInteractable_WorldCollect.h"


// Sets default values
AAbyssInteractable_WorldCollect::AAbyssInteractable_WorldCollect()
{
	
}

void AAbyssInteractable_WorldCollect::GatherInteractionOptions(const FInteractionQuery& InteractionQuery,
	FInteractionOptionBuilder& OptionBuilder)
{
	OptionBuilder.AddInteractionOption(Option);
}

FInventoryPickup AAbyssInteractable_WorldCollect::GetPickupInventory() const
{
	return InventoryPickup;
}

