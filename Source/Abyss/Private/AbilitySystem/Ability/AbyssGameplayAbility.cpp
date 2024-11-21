// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/AbyssGameplayAbility.h"
#include "Abyss/AbyssGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "AbilitySystem/System/AbyssAbilitySystemComponent.h"
#include "Character/AbyssCharacter.h"
#include "Player/AbyssPlayerController.h"

UAbyssGameplayAbility::UAbyssGameplayAbility(const FObjectInitializer& ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	
	ActivationPolicy = EAbyssAbilityActivationPolicy::OnInputTriggered;
	ActivationGroup = EAbyssAbilityActivationGroup::Independent;
}

AAbyssCharacter* UAbyssGameplayAbility::GetAbyssCharacterFromActorInfo() const
{
	return CurrentActorInfo ? CastChecked<AAbyssCharacter>(CurrentActorInfo->AvatarActor) : nullptr;
}

AAbyssPlayerController* UAbyssGameplayAbility::GetAbyssPlayerControllerFromActorInfo() const
{
	return CurrentActorInfo ? CastChecked<AAbyssPlayerController>(CurrentActorInfo->PlayerController) : nullptr;
}

bool UAbyssGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	UAbyssAbilitySystemComponent* AbyssASC = CastChecked<UAbyssAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	if (AbyssASC->IsActivationGroupBlocked(ActivationGroup))
	{
		if (OptionalRelevantTags)
		{
			OptionalRelevantTags->AddTag(AbyssGameplayTags::Ability_ActivateFail_ActivationGroup);
		}
		return false;
	}

	return true;
}

void UAbyssGameplayAbility::SetCanBeCanceled(bool bCanBeCanceled)
{
	// The ability can not block canceling if it's replaceable.
	if (!bCanBeCanceled && (ActivationGroup == EAbyssAbilityActivationGroup::Exclusive_Replaceable))
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("SetCanBeCanceled: Ability [%s] can not block canceling because its activation group is replaceable."), *GetName());
		return;
	}

	Super::SetCanBeCanceled(bCanBeCanceled);
}

void UAbyssGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	K2_OnAbilityAdded();

	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UAbyssGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAbyssGameplayAbility::ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec,
	FGameplayAbilitySpec* AbilitySpec) const
{
	Super::ApplyAbilityTagsToGameplayEffectSpec(Spec, AbilitySpec);

	//TODO : 
}

void UAbyssGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo,
                                                      const FGameplayAbilitySpec& Spec) const
{
	//const bool bIsPredicting = (Spec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting);
		// && (Spec.IsActive() && Spec.Ability->IsInstantiated());

	if (ActorInfo && !Spec.IsActive() && (ActivationPolicy == EAbyssAbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
		{
			bool bSuccess = ASC->TryActivateAbility(Spec.Handle);
			if (bSuccess)
			{
				UE_LOG(LogAbilitySystemComponent, Display,
				TEXT("UAbyssGameplayAbility : Success to Activate Ability[%s] On Spawn"), *GetNameSafe(Spec.Ability));
			}
		}
	}
}

void UAbyssGameplayAbility::OnPawnAvatarSet()
{
	K2_OnPawnAvatarSet();
}
