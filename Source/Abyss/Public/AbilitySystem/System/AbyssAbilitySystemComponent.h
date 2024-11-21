// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Ability/AbyssGameplayAbility.h"
#include "AbyssAbilitySystemComponent.generated.h"

class UAbyssAbilityTagRelationshipMapping;

UCLASS()
class ABYSS_API UAbyssAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

	friend class UAbyssGameplayAbility;

public:
	
	UAbyssAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	
	void SetTagRelationshipMapping(UAbyssAbilityTagRelationshipMapping* NewMapping){TagRelationshipMapping = NewMapping;}

	//将InputConfig的InputTag与注册的GA进行配对;并将成功配对的AbilitySpec收集到InputSpecHandles
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	bool IsActivationGroupBlocked(EAbyssAbilityActivationGroup Group) const;
	
private:

	void TryActivateAbilitiesOnSpawn();

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	
protected:

	UPROPERTY()
	TObjectPtr<UAbyssAbilityTagRelationshipMapping> TagRelationshipMapping;

	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	int32 ActivationGroupCounts[(int32)EAbyssAbilityActivationGroup::MAX];
};

