// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_GrantNearbyInteraction.generated.h"


UCLASS()
class UAbilityTask_GrantNearbyInteraction final : public UAbilityTask
{
	GENERATED_BODY()
public:

	UAbilityTask_GrantNearbyInteraction(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category="Ability|Task", meta=(HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = true))
	static UAbilityTask_GrantNearbyInteraction* GrantAbilitiesForNearbyInteractors(UGameplayAbility* OwningAbility, float InteractionScanRange, float InteractionScanRate);

private:
	
	virtual void OnDestroy(bool bInOwnerFinished) override;

	void QueryInteractables();

	float InteractionScanRange = 100.0f;
	float InteractionScanRate = 0.100f;

	FTimerHandle QueryTimerHandle;

	TMap<FObjectKey, FGameplayAbilitySpecHandle> InteractionAbilityCache;
};
