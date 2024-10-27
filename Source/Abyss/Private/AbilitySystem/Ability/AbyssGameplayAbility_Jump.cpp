// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/AbyssGameplayAbility_Jump.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Character/AbyssCharacter.h"

UAbyssGameplayAbility_Jump::UAbyssGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)	
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UAbyssGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                                    const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if(!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const AAbyssCharacter* AbyssCharacter = Cast<AAbyssCharacter>(ActorInfo->AvatarActor.Get());
	if(!AbyssCharacter || !AbyssCharacter->CanJump())
	{
		return false;
	}

	if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	return true;
}

void UAbyssGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	CharacterJumpStop();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAbyssGameplayAbility_Jump::CharacterJumpStart()
{
	if(AAbyssCharacter* AbyssCharacter = GetAbyssCharacterFromActorInfo())
	{
		if(AbyssCharacter->IsLocallyControlled() && !AbyssCharacter->bPressedJump)
		{
			AbyssCharacter->UnCrouch();
			AbyssCharacter->Jump();
		}	
	}
}

void UAbyssGameplayAbility_Jump::CharacterJumpStop()
{
	if(AAbyssCharacter* AbyssCharacter = GetAbyssCharacterFromActorInfo())
	{
		if(AbyssCharacter->IsLocallyControlled() && AbyssCharacter->bPressedJump)
		{
			AbyssCharacter->StopJumping();
		}
	}
}
