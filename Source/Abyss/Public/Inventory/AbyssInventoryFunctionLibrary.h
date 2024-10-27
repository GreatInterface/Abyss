// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbyssInventoryFunctionLibrary.generated.h"


class UAbyssInventoryItemDefinition;
class UAbyssInventoryItemFragment;

UCLASS()
class ABYSS_API UAbyssInventoryFunctionLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	static const UAbyssInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UAbyssInventoryItemDefinition> ItemDef, TSubclassOf<UAbyssInventoryItemFragment> FragmentClass);  
};
