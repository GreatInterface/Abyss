// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbyssInteractable.h"
#include "Inventory/Pickup/Pickupable.h"
#include "AbyssInteractable_WorldCollect.generated.h"

UCLASS(Abstract, Blueprintable)
class ABYSSCORERUNTIME_API AAbyssInteractable_WorldCollect : public AAbyssInteractable, public IPickupable
{
	GENERATED_BODY()

public:
	
	AAbyssInteractable_WorldCollect();

	virtual void GatherInteractionOptions(const FInteractionQuery& InteractionQuery, FInteractionOptionBuilder& OptionBuilder) override;
	virtual FInventoryPickup GetPickupInventory() const override;

private:
	
	UPROPERTY(EditAnywhere, Category="Abyss|Inventory")
	FInventoryPickup InventoryPickup;
};
