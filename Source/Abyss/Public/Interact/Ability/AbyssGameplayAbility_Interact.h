// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "AbilitySystem/Ability/AbyssGameplayAbility.h"
#include "Interact/Task/AbilityTask_GrantNearbyInteraction.h"
#include "Interact/Message/AbyssInteractMessageType.h"
#include "AbyssGameplayAbility_Interact.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Interaction_Activate);


class UIndicatorDescriptor;
struct FInteractionOption;


UCLASS()
class ABYSS_API UAbyssGameplayAbility_Interact : public UAbyssGameplayAbility
{
	GENERATED_BODY()

public:

	UAbyssGameplayAbility_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable)
	void UpdateInteractions(const TArray<FInteractionOption>& InteractionOptions);

	UFUNCTION(BlueprintCallable)
	void TriggerInteraction();

	bool HandleEquipment(UAbilitySystemComponent* ASC);
	void HandleCommonInteractable(const FInteractionOption& Option, FGameplayEventData Payload,
	                              FGameplayAbilityActorInfo ActorInfo);

protected:
	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRate = 0.1f;

	UPROPERTY(EditDefaultsOnly)
	float InteractionScanRange = 500.f;

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UUserWidget> DefaultInteractionWidgetClass;

protected:
	UFUNCTION(BlueprintCallable)
	void SetEquippedMessage(const FAbyssEquippedMessage& InMessage) { EquippedMessage = InMessage; }
	
protected:
//----------------------------------------------------------------------------------------
//		Equipment
//----------------------------------------------------------------------------------------
	
	UPROPERTY(BlueprintReadOnly)
	FAbyssEquippedMessage EquippedMessage; 
	
	UPROPERTY(EditDefaultsOnly, Category=Interact)
	TSubclassOf<UAbyssGameplayAbility> UnequipAbility;
	FGameplayAbilitySpecHandle UnequipAbilitySpecHandle;


	
	UPROPERTY(BlueprintReadWrite)
	TArray<FInteractionOption> CurrentOptions;
	
	UPROPERTY()
	TArray<TObjectPtr<UIndicatorDescriptor>> Indicators;

	UPROPERTY()
	TWeakObjectPtr<UAbilityTask_GrantNearbyInteraction> MyGrantNearbyInteractionTask;
	
};
