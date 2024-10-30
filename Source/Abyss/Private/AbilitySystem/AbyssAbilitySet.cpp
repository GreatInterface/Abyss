// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbyssAbilitySet.h"
#include "AbilitySystem/System/AbyssAbilitySystemComponent.h"
#include "AbilitySystem/Ability/AbyssGameplayAbility.h"
#include "Abyss/AbyssLogChannels.h"

//------------------------------------------------------------------------------------------------------------
//	FAbyssAbilitySet_GrantedHandles
//------------------------------------------------------------------------------------------------------------

class UAttributeSet;
struct FActiveGameplayEffectHandle;
struct FGameplayAbilitySpecHandle;


void FAbyssAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if(Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FAbyssAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if(Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FAbyssAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	if(Set)
	{
		GrantedAttributeSets.Add(Set);
	}
}

void FAbyssAbilitySet_GrantedHandles::TakeFromAbilitySystem(UAbyssAbilitySystemComponent* AbyssASC)
{
	check(AbyssASC)

	for(const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if(Handle.IsValid())
		{
			AbyssASC->ClearAbility(Handle);
		}
	}

	for(const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if(Handle.IsValid())
		{
			AbyssASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for(UAttributeSet* Set : GrantedAttributeSets)
	{
		AbyssASC->RemoveSpawnedAttribute(Set);
	}
}

//------------------------------------------------------------------------------------------------------------

UAbyssAbilitySet::UAbyssAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAbyssAbilitySet::GiveToAbilitySystem(UAbyssAbilitySystemComponent* AbyssASC,
	FAbyssAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(AbyssASC);

	// Grant the gameplay abilities.
	for(const FAbyssAbilitySet_GameplayAbility& GrantedAbility : GrantedGameplayAbilities)
	{
		if(!IsValid(GrantedAbility.Ability))
		{
			UE_LOG(LogAbyss, Error, TEXT("GrantedGameplayAbility [%s] on Ability Set [%s] Is not Valid"), *GetNameSafe(GrantedAbility.Ability), *GetNameSafe(this));
			continue;
		}

		// UAbyssGameplayAbility* AbilityCDO = GrantedAbility.Ability->GetDefaultObject<UAbyssGameplayAbility>();
		UAbyssGameplayAbility* Ability = NewObject<UAbyssGameplayAbility>(AbyssASC, GrantedAbility.Ability);
		
		FGameplayAbilitySpec AbilitySpec(Ability, GrantedAbility.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(GrantedAbility.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = AbyssASC->GiveAbility(AbilitySpec);
		
		UE_LOG(LogAbilitySystemComponent, Display,
			TEXT("UAbyssAbilitySet : Success to Give Ability [%s] to ASC"), *GetNameSafe(AbilitySpec.Ability));
		
		if(OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// Grant the gameplay effects.
	for(const FAbyssAbilitySet_GameplayEffect& GrantedEffect : GrantedGameplayEffects)
	{
		if(!IsValid(GrantedEffect.GameplayEffect))
		{
			UE_LOG(LogAbyss, Error, TEXT("GrantedGameplayEffect [%s] on Ability Set [%s] Is not Valid"), *GetNameSafe(GrantedEffect.GameplayEffect), *GetNameSafe(this));
			continue;
		}

		const UGameplayEffect* GameplayEffect = GrantedEffect.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		
		const FActiveGameplayEffectHandle Handle = AbyssASC->ApplyGameplayEffectToSelf(GameplayEffect, GrantedEffect.EffectLevel, AbyssASC->MakeEffectContext());
		if(OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(Handle);
		}
	}

	// Grant the attribute sets.
	for(const FAbyssAbilitySet_AttributeSet& GrantedSet : GrantedAttributeSets)
	{
		if(!IsValid(GrantedSet.AttributeSet))
		{
			UE_LOG(LogAbyss, Error, TEXT("GrantedAttributeSet [%s] on Ability Set [%s] Is not Valid"), *GetNameSafe(GrantedSet.AttributeSet), *GetNameSafe(this));
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(AbyssASC->GetOwner(), GrantedSet.AttributeSet);
		AbyssASC->AddAttributeSetSubobject(NewSet);

		if(OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}

