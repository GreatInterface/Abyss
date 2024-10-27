// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/GameplayTag/GameplayTagStack.h"
#include "UObject/Object.h"
#include "AbyssInventoryItemInstance.generated.h"

class UAbyssInventoryItemFragment;
class UAbyssInventoryItemDefinition;

/**
 * 
 */
UCLASS(BlueprintType)
class ABYSS_API UAbyssInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:

	UAbyssInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	//~Object 
	virtual void PostInitProperties() override;
	//~End of Object 
	
	UFUNCTION(BlueprintCallable, Category=Inventory)
	void AddStatTagStack(const FGameplayTag& InTag, int32 StackCount);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	void RemoveStatTagStack(const FGameplayTag& InTag, int32 StackCount);

	UFUNCTION(BlueprintCallable, Category=Inventory) 
	int32 GetStatTagStackCount(const FGameplayTag& InTag) const ;

	UFUNCTION(BlueprintCallable, Category=Inventory) 
	bool HasStatTag(const FGameplayTag& InTag) const;

	TSubclassOf<UAbyssInventoryItemDefinition> GetItemDefinition() const
	{
		return ItemDef;
	}
	void SetItemDefinition(TSubclassOf<UAbyssInventoryItemDefinition> InDefinition) { ItemDef = InDefinition; }

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(DeterminesOutputType=FragmentClass))
	const UAbyssInventoryItemFragment* FindFragmentByClass(TSubclassOf<UAbyssInventoryItemFragment> FragmentClass);

	template<typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return CastChecked<ResultClass*>(FindFragmentByClass(ResultClass::StaticClass()));
	}

private:

	UPROPERTY()
	FGameplayTagStackContainer StatTagStackContainer;

	UPROPERTY()
	TSubclassOf<UAbyssInventoryItemDefinition> ItemDef;
};
