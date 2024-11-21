// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Task/AbilityTask_WaitForInteractableTargets_SingleLineTrace.h"
#include "InteractionLibrary.generated.h"


class UAbilityTask_WaitForInteractableTargets_SingleLineTrace;
class IInteractableTarget;

UCLASS(meta=(ScriptName="Interaction"))
class ABYSS_API UInteractionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UInteractionLibrary();

public:

	UFUNCTION(BlueprintCallable)
	static AActor* GetActorFromInteractableTarget(TScriptInterface<IInteractableTarget> InteractableTarget);
	
	static void AppendInteractableTargetsFromOverlapResults(const TArray<FOverlapResult>& OverlapResults, OUT TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets);
	static void AppendInteractableTargetsFromHitResult(const FHitResult& HitResult, TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets);
};
