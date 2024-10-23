// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbyssGameplayAbility.h"
#include "AbyssGameplayAbility_Jump.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class UAbyssGameplayAbility_Jump : public UAbyssGameplayAbility
{
	GENERATED_BODY()

public:

	UAbyssGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION(BlueprintCallable, Category="Abyss|Ability")
	void CharacterJumpStart();

	UFUNCTION(BlueprintCallable, Category="Abyss|Ability")
	void CharacterJumpStop();	
};
