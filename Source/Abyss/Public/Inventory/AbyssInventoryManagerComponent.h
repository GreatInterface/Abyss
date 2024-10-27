// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbyssInventoryItemDefinition.h"
#include "Components/ActorComponent.h"
#include "AbyssInventoryManagerComponent.generated.h"

class UActorComponent;
class UAbyssInventoryItemInstance;

USTRUCT(BlueprintType)
struct FAbyssInventoryChangeMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UAbyssInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 Delta = 0;
};

USTRUCT(BlueprintType)
struct FAbyssInventoryEntry
{
	GENERATED_BODY()

private:
	friend struct FAbyssInventoryEntryContainer;

	UPROPERTY()
	TObjectPtr<UAbyssInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY()
	int32 LastObservedCount = INDEX_NONE;
};

USTRUCT(BlueprintType)
struct FAbyssInventoryEntryContainer
{
	GENERATED_BODY()

	FAbyssInventoryEntryContainer()
		: OwnerComponent(nullptr)
	{}

	FAbyssInventoryEntryContainer(UActorComponent* InOwnerComp)
		: OwnerComponent(InOwnerComp)
	{}

	TArray<UAbyssInventoryItemInstance*> GetAllItems() const;

	[[nodiscard]] UAbyssInventoryItemInstance* AddEntry(TSubclassOf<UAbyssInventoryItemDefinition> ItemClass, int32 StackCount);
	void AddEntry(UAbyssInventoryItemInstance* Instance);

	void RemoveEntry(UAbyssInventoryItemInstance* Instance);

private:
	void BroadcastChangeMessage(FAbyssInventoryEntry& Entry, int32 OldCount, int32 NewCount);
	
private:
	UPROPERTY()
	TArray<FAbyssInventoryEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
};


UCLASS(BlueprintType)
class ABYSS_API UAbyssInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAbyssInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool CanAddItemDefinition(TSubclassOf<UAbyssInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	UAbyssInventoryItemInstance* AddItemDefinition(TSubclassOf<UAbyssInventoryItemDefinition> ItemDef, int32 StackCount = 1);
	
	UFUNCTION(BlueprintCallable, Category=Inventory)
	void AddItemInstance(UAbyssInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	void RemoveItemInstance(UAbyssInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Inventory)
	TArray<UAbyssInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Inventory)
	UAbyssInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<UAbyssInventoryItemDefinition> ItemDef) const;

	int32 GetTotalItemCountByDefinition(TSubclassOf<UAbyssInventoryItemDefinition> ItemDef);
	bool ConsumeItemsByDefinition(TSubclassOf<UAbyssInventoryItemDefinition> ItemDef, int32 NumToConsume);
	
private:
	UPROPERTY()
	FAbyssInventoryEntryContainer InventoryList;
};
