// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbyssAbilityTagRelationshipMapping.generated.h"

USTRUCT()
struct FAbyssAbilityTagRelationship
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Ability, meta=(Categories="Gameplay.Action"))
	FGameplayTag AbilityTag;

	//任何使用此标签的能力将阻止的其他能力标签
	UPROPERTY(EditAnywhere, Category=Ability)
	FGameplayTagContainer AbilityTagsToBlock;
	
	UPROPERTY(EditAnywhere, Category=Ability)
	FGameplayTagContainer AbilityTagsToCancel;

	/** 如果一个能力具有此标签，则此标签将隐式添加到该能力的激活所需标签中 */
	UPROPERTY(EditAnywhere, Category=Ability)
	FGameplayTagContainer ActivationRequiredTags;

	/** 如果一个能力具有此标签，则此标签将隐式添加到该能力的激活阻止标签中 */
	UPROPERTY(EditAnywhere, Category=Ability)
	FGameplayTagContainer ActivationBlockedTags;
};

UCLASS()
class ABYSS_API UAbyssAbilityTagRelationshipMapping : public UDataAsset
{
	GENERATED_BODY()

public:

	void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const;

	void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const;

	bool IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const;
	
private:
	UPROPERTY(EditAnywhere, Category=Ability, meta=(TitleProperty="AbilityTag"))
	TArray<FAbyssAbilityTagRelationship> AbilityTagRelationships;
};
