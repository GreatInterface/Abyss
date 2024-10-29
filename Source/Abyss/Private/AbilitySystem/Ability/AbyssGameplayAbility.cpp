// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/AbyssGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Character/AbyssCharacter.h"
#include "Player/AbyssPlayerController.h"

UAbyssGameplayAbility::UAbyssGameplayAbility(const FObjectInitializer& ObjectInitializer)
{
	ActivationPolicy = EAbyssAbilityActivationPolicy::OnInputTriggered;
}

AAbyssCharacter* UAbyssGameplayAbility::GetAbyssCharacterFromActorInfo() const
{
	return CurrentActorInfo ? CastChecked<AAbyssCharacter>(CurrentActorInfo->AvatarActor) : nullptr;
}

AAbyssPlayerController* UAbyssGameplayAbility::GetAbyssPlayerControllerFromActorInfo() const
{
	return CurrentActorInfo ? CastChecked<AAbyssPlayerController>(CurrentActorInfo->PlayerController) : nullptr;
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
	const bool bIsPredicting = (Spec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting);
		// && (Spec.IsActive() && Spec.Ability->IsInstantiated());

	if (ActorInfo && !Spec.IsActive() && !bIsPredicting && (ActivationPolicy == EAbyssAbilityActivationPolicy::OnSpawn))
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
