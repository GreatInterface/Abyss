// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbyssAbilityTagRelationshipMapping.h"

void UAbyssAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags,
	FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const
{
	for(int32 Idx = 0; Idx < AbilityTagRelationships.Num(); ++Idx)
	{
		const FAbyssAbilityTagRelationship& Ship = AbilityTagRelationships[ Idx ];
		if(AbilityTags.HasTag(Ship.AbilityTag))
		{
			if(OutTagsToBlock)
			{
				OutTagsToBlock->AppendTags(Ship.AbilityTagsToBlock);
			}
			if(OutTagsToCancel)
			{
				OutTagsToCancel->AppendTags(Ship.AbilityTagsToCancel);
			}
		}
	}
}

void UAbyssAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags,
	FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const
{
	for(int32 Idx = 0; Idx < AbilityTagRelationships.Num(); ++Idx)
	{
		const FAbyssAbilityTagRelationship& Ship = AbilityTagRelationships[ Idx ];
		if(AbilityTags.HasTag(Ship.AbilityTag))
		{
			if(OutActivationBlocked)
			{
				OutActivationBlocked->AppendTags(Ship.ActivationBlockedTags);
			}
			if(OutActivationRequired)
			{
				OutActivationRequired->AppendTags(Ship.ActivationRequiredTags);
			}
		}
	}
}

bool UAbyssAbilityTagRelationshipMapping::IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags,
	const FGameplayTag& ActionTag) const
{
	for(int32 Idx = 0; Idx < AbilityTagRelationships.Num(); ++Idx)
	{
		const FAbyssAbilityTagRelationship& Ship = AbilityTagRelationships[ Idx ];

		if(Ship.AbilityTag == ActionTag && Ship.AbilityTagsToCancel.HasAny(AbilityTags))
		{
			return true;
		}
	}

	return false;
}
