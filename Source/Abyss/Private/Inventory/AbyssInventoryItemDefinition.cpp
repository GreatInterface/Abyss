// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/AbyssInventoryItemDefinition.h"

UAbyssInventoryItemDefinition::UAbyssInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UAbyssInventoryItemFragment* UAbyssInventoryItemDefinition::FindFragmentByClass(
	TSubclassOf<UAbyssInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass)
	{
		for (UAbyssInventoryItemFragment* Entry : Fragments)
		{
			if (Entry && Entry->IsA(FragmentClass))
			{
				return Entry;
			}
		}
	}

	return nullptr;
}
