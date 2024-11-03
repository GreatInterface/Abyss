// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "AbilitySystem/AbyssAbilitySet.h"
#include "GameFeatureAction_AddAbilities.generated.h"

struct FComponentRequestHandle;
struct FGameplayAbilitySpecHandle;
class  UAbyssAbilitySet;
class  UAttributeSet;
class  UGameplayAbility;

USTRUCT(BlueprintType)
struct FAbyssAbilityGrant
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles="Client"))
	TSoftClassPtr<UGameplayAbility> AbilityType;
};

USTRUCT(BlueprintType)
struct FAbyssAttributeSetGrant
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles="Client"))
	TSoftClassPtr<UAttributeSet> AttributeSetType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AssetBundles="Client"))
	TSoftObjectPtr<UDataTable> InitializationData;	
};


USTRUCT()
struct FGameFeatureAbilityEntry
{
	GENERATED_BODY()

	//the base class to add to
	UPROPERTY(EditAnywhere, Category="Ability")
	TSoftClassPtr<AActor> ActorClass;

	UPROPERTY(EditAnywhere, Category="Ability")
	TArray<FAbyssAbilityGrant> GrantedAbilities;

	UPROPERTY(EditAnywhere, Category="Attributes")
	TArray<FAbyssAttributeSetGrant> GrantedAttributes;

	UPROPERTY(EditAnywhere, Category="Attributes", meta=(AssetBundles="Client"))
	TArray<TSoftObjectPtr<const UAbyssAbilitySet>> GrantedAbilitySets;
};


/**
 *   UGameFeatureAction_AddAbilities
 */
UCLASS(MinimalAPI, meta=(DisplayName = "Add Abilities"))
class UGameFeatureAction_AddAbilities final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:

	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	UPROPERTY(EditAnywhere, Category="Ability", meta=(TitleProperty="ActorClass", ShowOnlyInnerProperties))
	TArray<FGameFeatureAbilityEntry> AbilityList;

private:
	struct FActorExtensions
	{
		TArray<FGameplayAbilitySpecHandle> Abilities;
		TArray<UAttributeSet*> Attributes;
		TArray<FAbyssAbilitySet_GrantedHandles> AbilitySetHandles;
	};

	struct FPerContextData
	{
		TMap<AActor*, FActorExtensions> ActiveExtensions;
		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;
	};

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

	void Reset(FPerContextData& ActiveData);
	void HandleActorExtension(AActor* Actor, FName EventName, int32 EntryIndex, FGameFeatureStateChangeContext ChangeContext);
	void AddActorAbilities(AActor* Actor, const FGameFeatureAbilityEntry& AbilityEntry, FPerContextData& ActiveData);
	void RemoveActorAbilities(AActor* Actor, FPerContextData& ActiveData);

	template <class ComponentType>
	ComponentType* FindOrAddComponentForActor(AActor* Actor, const FGameFeatureAbilityEntry& Entry, FPerContextData& ActiveData)
	{
		return Cast<ComponentType>(FindOrAddComponentForActor(ComponentType::StaticClass(), Actor, Entry, ActiveData));
	}

	UActorComponent* FindOrAddComponentForActor(UClass* ComponentType, AActor* Actor, const FGameFeatureAbilityEntry& Entry, FPerContextData& ActiveData);

};
