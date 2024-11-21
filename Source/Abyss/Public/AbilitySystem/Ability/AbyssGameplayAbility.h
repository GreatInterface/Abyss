// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbyssGameplayAbility.generated.h"

class AAbyssPlayerController;
class AAbyssCharacter;

UENUM(BlueprintType)
enum class EAbyssAbilityActivationPolicy : uint8
{
	OnInputTriggered,

	WhileInputActive,

	OnSpawn
};

//ELyraAbilityActivationGroup（能力激活组
//定义一种能力与其他能力的激活关系。
UENUM(BlueprintType)
enum class EAbyssAbilityActivationGroup : uint8
{
	// Ability runs independently of all other abilities.
	Independent,

	// Ability is canceled and replaced by other exclusive abilities.
	Exclusive_Replaceable,

	// Ability blocks all other exclusive abilities from activating.
	Exclusive_Blocking,

	MAX	UMETA(Hidden)
};


UCLASS(Abstract, HideCategories = Input, meta=(ShortTooltip = "The base gameplay ability class used by this project."))
class ABYSS_API UAbyssGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class UAbyssAbilitySystemComponent;

public:

	UAbyssGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Abyss|Ability")
	AAbyssCharacter* GetAbyssCharacterFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category="Abyss|Ability")
	AAbyssPlayerController* GetAbyssPlayerControllerFromActorInfo() const;
	
	EAbyssAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	
protected:
	//~UGameplayAbility Interface
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const override;
	//~End of UGameplayAbility Interface

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	virtual void OnPawnAvatarSet();

	UFUNCTION(BlueprintImplementableEvent, Category="Ability", DisplayName="OnAbilityAdded")
	void K2_OnAbilityAdded();

	UFUNCTION(BlueprintImplementableEvent, Category="Ability", DisplayName="OnAbilityRemoved")
	void K2_OnAbilityRemoved();

	/** Called when the ability system is initialized with a pawn avatar. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvatarSet")
	void K2_OnPawnAvatarSet();

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Abyss|Ability Activation")
	EAbyssAbilityActivationPolicy ActivationPolicy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Abyss|Ability Activation")
	EAbyssAbilityActivationGroup ActivationGroup;
	
};
