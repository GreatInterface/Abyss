// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/AbyssInventoryFunctionLibrary.h"
#include "Inventory/AbyssInventoryItemDefinition.h"

const UAbyssInventoryItemFragment* UAbyssInventoryFunctionLibrary::FindItemDefinitionFragment(
	TSubclassOf<UAbyssInventoryItemDefinition> ItemDef, TSubclassOf<UAbyssInventoryItemFragment> FragmentClass)
{
	if (ItemDef && FragmentClass)
	{
		return GetDefault<UAbyssInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}
