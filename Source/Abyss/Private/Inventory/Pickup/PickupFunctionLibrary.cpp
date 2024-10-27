// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Pickup/PickupFunctionLibrary.h"
#include "Inventory/AbyssInventoryManagerComponent.h"
#include "Inventory/Pickup/Pickupable.h"

TScriptInterface<IPickupable> UPickupFunctionLibrary::GetFirstPickupableFromActor(AActor* Actor)
{
	TScriptInterface<IPickupable> PickupActor(Actor);
	if (PickupActor)
	{
		return PickupActor;
	}

	TArray<UActorComponent*> PickupCompList = Actor ? Actor->FindComponentByInterface(UPickupable::StaticClass()) : TArray<UActorComponent*>();
	if (PickupCompList.Num() > 0)
	{
		return TScriptInterface<IPickupable>(PickupCompList[0]);
	}

	return TScriptInterface<IPickupable>();
}

void UPickupFunctionLibrary::AddPickupToInventory(UAbyssInventoryManagerComponent* InventoryManagerComponent,
	TScriptInterface<IPickupable> Pickup)
{
	if (InventoryManagerComponent && Pickup)
	{
		const FInventoryPickup& PickupInventory = Pickup->GetPickupInventory();

		for (const FPickupTemplate& Template : PickupInventory.Templates)
		{
			InventoryManagerComponent->AddItemDefinition(Template.ItemDef, Template.StackCount);
		}

		for (const FPickupInstance& Instance : PickupInventory.Instances)
		{
			InventoryManagerComponent->AddItemInstance(Instance.Item);
		}
	}
}
