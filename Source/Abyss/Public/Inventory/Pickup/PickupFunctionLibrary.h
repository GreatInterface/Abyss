// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PickupFunctionLibrary.generated.h"


class UAbyssInventoryManagerComponent;
class IPickupable;

UCLASS()
class ABYSS_API UPickupFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	static TScriptInterface<IPickupable> GetFirstPickupableFromActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, meta=(WorldContext = "Ability"))
	static void AddPickupToInventory(UAbyssInventoryManagerComponent* InventoryManagerComponent, TScriptInterface<IPickupable> Pickup);
};
