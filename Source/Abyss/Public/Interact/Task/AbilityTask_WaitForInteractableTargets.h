// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Interact/InteractionOption.h"
#include "AbilityTask_WaitForInteractableTargets.generated.h"

struct FInteractionQuery;
struct FInteractionOption;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableObjectsChangedEvent, const TArray<FInteractionOption>&, InteractableOptions);

UCLASS(Abstract)
class UAbilityTask_WaitForInteractableTargets : public UAbilityTask
{
	GENERATED_BODY()

public:

	UAbilityTask_WaitForInteractableTargets(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable)
	FInteractableObjectsChangedEvent InteractableObjectsChanged;

protected:

	static void LineTrace(FHitResult& OutHitResult, const UWorld* World, const FVector& Start, const FVector& End,
	                      FName ProfileName, const FCollisionQueryParams Params);

	void AimWithPlayerController(const AActor* InSourceActor, const FCollisionQueryParams& Params,
	                             const FVector& TraceStart, float MaxRange, FVector& OutTraceEnd,
	                             bool bIgnorePitch = false) const;

 	/**
 	 * @param ClippedPosition : (OUT) 摄像机射线与AbilityCenter为圆心AbilityRange为半径的球体 的交点(默认为“入口点”)
 	 * @return 如果没有交点(见上面的ClippedPosition)，则返回false
 	 */
	static bool ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter,
	                                        float AbilityRange, FVector& ClippedPosition);

	void UpdateInteractableOptions(const FInteractionQuery& InteractQuery,
	                               const TArray<TScriptInterface<IInteractableTarget>>& InteractableTargets);

protected:

	FCollisionProfileName TraceProfile;

	bool bTraceAffectsAimPitch = true;

	TArray<FInteractionOption> CurrentOptions;
};
