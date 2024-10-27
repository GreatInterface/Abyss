// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AbyssInventoryItemDefinition.generated.h"

class UAbyssInventoryItemInstance;
/**
 *  DefaultToInstanced :
 *		指定该类的所有实例属性都默认是UPROPERTY(instanced)，即都默认创建新的实例，而不是对对象的引用。
 *
 *	EditInlineNew : 
 *		指定该类的对象可以在属性细节面板里直接内联创建，要和属性的Instanced配合。
 *
 *  Fragment : 片段
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class ABYSS_API UAbyssInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UAbyssInventoryItemInstance* Instance) const {}
};

//////////////////////////////////////////////////////////////////////////////////////////////
///

UCLASS(Blueprintable, Const, Abstract)
class UAbyssInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UAbyssInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const UAbyssInventoryItemFragment* FindFragmentByClass(TSubclassOf<UAbyssInventoryItemFragment> FragmentClass) const;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<UAbyssInventoryItemFragment>> Fragments;
};

