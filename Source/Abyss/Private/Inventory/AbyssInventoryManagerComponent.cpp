// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/AbyssInventoryManagerComponent.h"
#include "Abyss/AbyssLogChannels.h"
#include "Inventory/AbyssInventoryItemInstance.h"


TArray<UAbyssInventoryItemInstance*> FAbyssInventoryEntryContainer::GetAllItems() const
{
	TArray<UAbyssInventoryItemInstance*> ResultList;
	ResultList.Reserve(Entries.Num());

	for (const FAbyssInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance)
		{
			ResultList.Add(Entry.Instance);
		}
	}

	return ResultList;
}

UAbyssInventoryItemInstance* FAbyssInventoryEntryContainer::AddEntry(
	TSubclassOf<UAbyssInventoryItemDefinition> ItemClass, int32 StackCount)
{
	FAbyssInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UAbyssInventoryItemInstance>(OwnerComponent->GetOwner());
	NewEntry.Instance->SetItemDefinition(ItemClass);
	NewEntry.StackCount = StackCount;

	for (UAbyssInventoryItemFragment* Fragment : GetDefault<UAbyssInventoryItemDefinition>(ItemClass)->Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}

	return NewEntry.Instance;
}

void FAbyssInventoryEntryContainer::AddEntry(UAbyssInventoryItemInstance* Instance)
{
	FAbyssInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	if (Instance)
	{
		NewEntry.Instance = Instance;
		NewEntry.StackCount = 1;
	}
	else
	{
		UE_LOG(LogAbyss, Error, TEXT("[InventoryEntryContainer] : Instance 为空指针"));
	}
}

void FAbyssInventoryEntryContainer::RemoveEntry(UAbyssInventoryItemInstance* Instance)
{
	auto EntryIt = Entries.CreateIterator(); 
	for (; EntryIt; ++EntryIt)
	{
		if (EntryIt->Instance == Instance)
		{
			EntryIt.RemoveCurrent();
		}
	}
}

void FAbyssInventoryEntryContainer::BroadcastChangeMessage(FAbyssInventoryEntry& Entry, int32 OldCount, int32 NewCount)
{
	FAbyssInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	// TODO : UGameplayMessage
}

UAbyssInventoryManagerComponent::UAbyssInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
}

bool UAbyssInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<UAbyssInventoryItemDefinition> ItemDef,
	int32 StackCount)
{
	//TODO :  Add support for stack limit / uniqueness checks / etc...
	if (ItemDef)
	{
		return true;
	}

	return false;
}

UAbyssInventoryItemInstance* UAbyssInventoryManagerComponent::AddItemDefinition(
	TSubclassOf<UAbyssInventoryItemDefinition> ItemDef, int32 StackCount)
{
	if (CanAddItemDefinition(ItemDef, StackCount))
	{
		UAbyssInventoryItemInstance* Result = InventoryList.AddEntry(ItemDef, StackCount);
		return Result;
	}

	return nullptr;
}

void UAbyssInventoryManagerComponent::AddItemInstance(UAbyssInventoryItemInstance* ItemInstance)
{
	if (ItemInstance)
	{
		InventoryList.AddEntry(ItemInstance);
	}
}

void UAbyssInventoryManagerComponent::RemoveItemInstance(UAbyssInventoryItemInstance* ItemInstance)
{
	if (ItemInstance)
	{
		InventoryList.RemoveEntry(ItemInstance);
	}
}

TArray<UAbyssInventoryItemInstance*> UAbyssInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

UAbyssInventoryItemInstance* UAbyssInventoryManagerComponent::FindFirstItemStackByDefinition(
	TSubclassOf<UAbyssInventoryItemDefinition> ItemDef) const
{
	for (UAbyssInventoryItemInstance* Instance : GetAllItems())
	{
		if (IsValid(Instance))
		{
			if (Instance->GetItemDefinition() == ItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

int32 UAbyssInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<UAbyssInventoryItemDefinition> ItemDef)
{
	int32 TotalCount = 0;
	for (UAbyssInventoryItemInstance* Instance : GetAllItems())
	{
		if (IsValid(Instance))
		{
			if (Instance->GetItemDefinition() == ItemDef)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

bool UAbyssInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<UAbyssInventoryItemDefinition> ItemDef,
	int32 NumToConsume)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (UAbyssInventoryItemInstance* Instance = FindFirstItemStackByDefinition(ItemDef))
		{
			InventoryList.RemoveEntry(Instance);
			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
}






