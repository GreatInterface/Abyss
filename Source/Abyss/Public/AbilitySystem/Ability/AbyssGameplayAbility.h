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
	
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
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
	
};
