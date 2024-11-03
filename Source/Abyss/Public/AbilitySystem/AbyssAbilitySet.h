// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbyssAbilitySet.generated.h"

class UAbyssAbilitySystemComponent;
struct FActiveGameplayEffectHandle;
struct FGameplayAbilitySpecHandle;
class UAttributeSet;
class UGameplayEffect;
class UAbyssGameplayAbility;

USTRUCT(BlueprintType)
struct FAbyssAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAbyssGameplayAbility> Ability = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly, meta=(Categories="InputTag"))
	FGameplayTag InputTag;
};

//------------------------------------------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct FAbyssAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.f;
};

//------------------------------------------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct FAbyssAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;
};

//------------------------------------------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct FAbyssAbilitySet_GrantedHandles
{
	GENERATED_BODY()
public:

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);

	void TakeFromAbilitySystem(UAbyssAbilitySystemComponent* AbyssASC);

protected:

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

//------------------------------------------------------------------------------------------------------------

UCLASS(BlueprintType, Const)
class ABYSS_API UAbyssAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UAbyssAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	void GiveToAbilitySystem(UAbyssAbilitySystemComponent* AbyssASC, FAbyssAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;
	
protected:

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities", meta=(TitleProperty=Ability))
	TArray<FAbyssAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects", meta=(TitleProperty=GameplayEffect))
	TArray<FAbyssAbilitySet_GameplayEffect> GrantedGameplayEffects;

	UPROPERTY(EditDefaultsOnly, Category="Attribute Sets", meta=(TitleProperty=AttributeSet))
	TArray<FAbyssAbilitySet_AttributeSet> GrantedAttributeSets;
};
