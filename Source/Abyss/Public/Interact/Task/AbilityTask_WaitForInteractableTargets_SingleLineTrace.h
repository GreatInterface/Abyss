// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityTask_WaitForInteractableTargets.h"
#include "Interact/InteractionQuery.h"
#include "AbilityTask_WaitForInteractableTargets_SingleLineTrace.generated.h"


UCLASS()
class ABYSS_API UAbilityTask_WaitForInteractableTargets_SingleLineTrace final : public UAbilityTask_WaitForInteractableTargets
{
	GENERATED_BODY()
public:
	
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly = true))
	static UAbilityTask_WaitForInteractableTargets_SingleLineTrace*
		WaitForInteractableTargets_SingleLineTrace(UGameplayAbility* OwningAbility,
			const FInteractionQuery& InteractionQuery, FCollisionProfileName TraceProfile,
			const FGameplayAbilityTargetingLocationInfo& StartLocation,
			float InteractionScanRange = 100, float InteractionScanRate = 0.1f, bool bShowDebug = false);

private:

	virtual void OnDestroy(bool bInOwnerFinished) override;

	void PerformTrace();
	
private:

	UPROPERTY()
	FInteractionQuery InteractionQuery;

	UPROPERTY()
	FGameplayAbilityTargetingLocationInfo StartLocation;

	float InteractionScanRange = 100.f;
	float InteractionScanRate  = 0.1f;

	bool bShowDebug = false;

	FTimerHandle TimerHandle;
};
