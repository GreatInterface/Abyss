// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/System/AbyssAbilitySystemComponent.h"

#include "AbilitySystemLog.h"
#include "AbilitySystem/Ability/AbyssGameplayAbility.h"
#include "Animation/AbyssAnimInstance.h"


UAbyssAbilitySystemComponent::UAbyssAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
{
	InputPressedSpecHandles. Reset();
	InputHeldSpecHandles.	 Reset();
	InputReleasedSpecHandles.Reset();
}

void UAbyssAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor); 
	
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if(bHasNewPawnAvatar)
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			UAbyssGameplayAbility* Ability = Cast<UAbyssGameplayAbility>(AbilitySpec.Ability);
			if (!Ability)
			{
				continue;
			}

			if (Ability->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
			{
				for (UGameplayAbility* Instance : AbilitySpec.GetAbilityInstances())
				{
					UAbyssGameplayAbility* AbyssAbilityInstance = Cast<UAbyssGameplayAbility>(Instance);
					if (AbyssAbilityInstance)
					{
						AbyssAbilityInstance->OnPawnAvatarSet();
					}
				}
			}
			else
			{
				Ability->OnPawnAvatarSet();
			}
		}

		//TODO : RegisterASC
		
		if(UAbyssAnimInstance* AbyssAnimInst = Cast<UAbyssAnimInstance>(ActorInfo->GetAnimInstance()))
		{
			AbyssAnimInst->InitializeWithAbilitySystem(this);
		}
	}

	TryActivateAbilitiesOnSpawn();
}

void UAbyssAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if(InputTag.IsValid())
	{
		for(const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if(AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
				ABILITY_LOG(Display, TEXT("ASC : [%s] Pressed to Avative Ability [%s]"), *InputTag.ToString(), *GetNameSafe(AbilitySpec.Ability));
			}
		}
	}
}

void UAbyssAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if(InputTag.IsValid())
	{
		for(const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if(AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UAbyssAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	//TODO : InputBlocked

	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	for(const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if(const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if(AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UAbyssGameplayAbility* AbyssAbilityCDO = Cast<UAbyssGameplayAbility>(AbilitySpec->Ability);
				if(AbyssAbilityCDO && AbyssAbilityCDO->GetActivationPolicy() == EAbyssAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	for(const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if(FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if(AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if(AbilitySpec->IsActive())
				{
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UAbyssGameplayAbility* Ability = Cast<UAbyssGameplayAbility>(AbilitySpec->Ability);
					if(Ability && Ability->GetActivationPolicy() == EAbyssAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	for(const FGameplayAbilitySpecHandle& SpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(SpecHandle);
	}

	for(const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if(FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if(AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if(AbilitySpec->IsActive())
				{
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}
	
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UAbyssAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UAbyssAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (const UAbyssGameplayAbility* AbyssAbility = Cast<UAbyssGameplayAbility>(Spec.Ability))
		{
			AbyssAbility->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), Spec);
		}
	}
}

void UAbyssAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);
}

void UAbyssAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);
}
