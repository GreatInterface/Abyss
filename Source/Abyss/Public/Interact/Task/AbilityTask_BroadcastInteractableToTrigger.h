// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_BroadcastInteractableToTrigger.generated.h"

class AAbyssInteractable;
class IInteractableTarget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractableToTriggerDelegate);

UCLASS()
class ABYSS_API UAbilityTask_BroadcastInteractableToTrigger : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAbilityTask_BroadcastInteractableToTrigger(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category="Ability|Task", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly=true))
	static UAbilityTask_BroadcastInteractableToTrigger* BroadcastInteractableToTrigger(UGameplayAbility* OwningAbility, AActor* InInstigator, const TArray<TSoftObjectPtr<AAbyssInteractable>>& Targets);


private:

	UPROPERTY()	
	TObjectPtr<AActor> Instigator;
	
	TArray<TSoftObjectPtr<AAbyssInteractable>> WaitTriggers;

	UPROPERTY(BlueprintAssignable)
	FInteractableToTriggerDelegate OnTrigger;
};