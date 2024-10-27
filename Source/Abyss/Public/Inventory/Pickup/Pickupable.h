// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupInstance.h"
#include "UObject/Interface.h"
#include "Pickupable.generated.h"


UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPickupable : public UInterface
{
	GENERATED_BODY()
};

class IPickupable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	virtual FInventoryPickup GetPickupInventory() const = 0;
};