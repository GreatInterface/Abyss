// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/AbyssInventoryItemInstance.h"

#include "Inventory/AbyssInventoryItemDefinition.h"

UAbyssInventoryItemInstance::UAbyssInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAbyssInventoryItemInstance::PostInitProperties()
{
	UObject::PostInitProperties();
	
	for (UAbyssInventoryItemFragment* Fragment : GetDefault<UAbyssInventoryItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(this);
		}
	}
}

void UAbyssInventoryItemInstance::AddStatTagStack(const FGameplayTag& InTag, int32 StackCount)
{
	StatTagStackContainer.AddStack(InTag, StackCount);
}

void UAbyssInventoryItemInstance::RemoveStatTagStack(const FGameplayTag& InTag, int32 StackCount)
{
	StatTagStackContainer.RemoveStack(InTag, StackCount);
}

int32 UAbyssInventoryItemInstance::GetStatTagStackCount(const FGameplayTag& InTag) const
{
	return StatTagStackContainer.GetStackCount(InTag);
}

bool UAbyssInventoryItemInstance::HasStatTag(const FGameplayTag& InTag) const
{
	return StatTagStackContainer.ContainsTag(InTag);
}

const UAbyssInventoryItemFragment* UAbyssInventoryItemInstance::FindFragmentByClass(
	TSubclassOf<UAbyssInventoryItemFragment> FragmentClass)
{
	if (ItemDef && FragmentClass)
	{
		return GetDefault<UAbyssInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}
