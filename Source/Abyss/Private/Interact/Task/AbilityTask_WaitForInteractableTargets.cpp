// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/Task/AbilityTask_WaitForInteractableTargets.h"

#include "AbilitySystemComponent.h"
#include "Interact/InteractableTarget.h"

UAbilityTask_WaitForInteractableTargets::UAbilityTask_WaitForInteractableTargets(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAbilityTask_WaitForInteractableTargets::LineTrace(FHitResult& OutHitResult, const UWorld* World,
                                                        const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params)
{
	check(World);

	OutHitResult = FHitResult(); 
	TArray<FHitResult> HitResults;
	World->LineTraceMultiByProfile(HitResults, Start, End, ProfileName, Params);
	
	if (HitResults.Num() > 0)
	{
		OutHitResult = HitResults[0];
	}
}

void UAbilityTask_WaitForInteractableTargets::AimWithPlayerController(const AActor* InSourceActor,
	const FCollisionQueryParams& Params, const FVector& TraceStart, float MaxRange, FVector& OutTraceEnd,
	bool bIgnorePitch) const
{
	if (!Ability) return;

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	check(PC);

	FVector ViewStart;
	FRotator ViewRot;
	PC->GetPlayerViewPoint(ViewStart, ViewRot);

	const FVector ViewDirection = ViewRot.Vector();
	FVector ViewEnd = ViewStart + (ViewDirection * MaxRange);

	ClipCameraRayToAbilityRange(ViewStart, ViewDirection, TraceStart,
		MaxRange, OUT ViewEnd);

	FHitResult HitResult;
	LineTrace(HitResult, InSourceActor->GetWorld(), ViewStart, ViewEnd, TraceProfile.Name, Params);

	const bool bUseTraceResult = HitResult.bBlockingHit && (FVector::DistSquared(TraceStart, HitResult.Location) <= (MaxRange * MaxRange));

	const FVector AdjustedEnd = bUseTraceResult ? HitResult.Location : ViewEnd;
	FVector AdjustedAimDir = (AdjustedEnd - TraceStart).GetSafeNormal();
	if (AdjustedAimDir.IsZero())
	{
		AdjustedAimDir = ViewDirection;
	}

	if (!bTraceAffectsAimPitch && bUseTraceResult)
	{
		FVector OriginalAimDir = (ViewEnd - TraceStart).GetSafeNormal();

		if (!OriginalAimDir.IsZero())
		{
			const FRotator OriginalAimRot = OriginalAimDir.Rotation();

			FRotator AdjustedAimRot = AdjustedAimDir.Rotation();
			AdjustedAimRot.Pitch = OriginalAimRot.Pitch;

			AdjustedAimDir = AdjustedAimRot.Vector();
		}
	}

	OutTraceEnd = TraceStart + (AdjustedAimDir * MaxRange);
}

bool UAbilityTask_WaitForInteractableTargets::ClipCameraRayToAbilityRange(FVector CameraLocation,
	FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition)
{
	FVector CameraToCenter = AbilityCenter - CameraLocation;

	// CameraDirection为单位向量
	// DotToCenter = |A||B|Cos(AB) = |A|Cos(AB) = CameraToCenter * Cos(AB)
	float DotToCenter = FVector::DotProduct(CameraToCenter, CameraDirection);

	//DotToCenter < 0这种情况一般是摄像机在角色身前
	if (DotToCenter >= 0)
	{
		//AbilityCenter到CameraDirection方向上的垂直距离的平方
		float DistanceSquared = CameraToCenter.SizeSquared() - (DotToCenter * DotToCenter);
		float RadiusSquared = (AbilityRange * AbilityRange);

		// 检测摄像机射线是否有穿过以Center为圆心,Radius为半径的球体
		if (DistanceSquared <= RadiusSquared)
		{
			float DistanceFromCamera = FMath::Sqrt(RadiusSquared - DistanceSquared);
			//射线从摄像机到球体表面的直线距离
			float DistanceAlongRay = DotToCenter + DistanceFromCamera;
			ClippedPosition = CameraLocation + (DistanceAlongRay * CameraDirection);
			return true;
		}
	}

	return false;
}

void UAbilityTask_WaitForInteractableTargets::UpdateInteractableOptions(const FInteractionQuery& InteractQuery,
	const TArray<TScriptInterface<IInteractableTarget>>& InteractableTargets)
{
	TArray<FInteractionOption> NewOptions;

	for (const TScriptInterface<IInteractableTarget>& Target : InteractableTargets)
	{
		TArray<FInteractionOption> TempOptions;
		FInteractionOptionBuilder Builder(Target, OUT TempOptions);
		Target->GatherInteractionOptions(InteractQuery, Builder);

		for (FInteractionOption& Option : TempOptions)
		{
			FGameplayAbilitySpec* Spec = nullptr;

			if (Option.TargetASC && Option.TargetInteractionAbilitySpecHandle.IsValid())
			{
				Spec = Option.TargetASC->FindAbilitySpecFromHandle(Option.TargetInteractionAbilitySpecHandle);
			}
			else if (Option.InteractionAbilityToGrant)
			{
				Spec = AbilitySystemComponent->FindAbilitySpecFromClass(Option.InteractionAbilityToGrant);

				if (Spec)
				{
					Option.TargetASC = AbilitySystemComponent.Get();
					Option.TargetInteractionAbilitySpecHandle = Spec->Handle;
				}
			}

			if (Spec)
			{
				if (Spec->Ability->CanActivateAbility(Spec->Handle, Option.TargetASC->AbilityActorInfo.Get()))
				{
					NewOptions.Add(Option);
				}
			}
		}
	}

	bool bOptionsChanges = false;
	if (NewOptions.Num() == CurrentOptions.Num())
	{
		NewOptions.Sort();

		for (auto It = NewOptions.CreateIterator(); It ; ++It)
		{
			const FInteractionOption& NewOption = *It;
			const FInteractionOption& CurrentOption = CurrentOptions[ It.GetIndex() ];

			if (NewOption != CurrentOption)
			{
				bOptionsChanges = true;
				break;
			}
		}
	}
	else
	{
		bOptionsChanges = true;
	}

	if (bOptionsChanges)
	{
		CurrentOptions = MoveTemp(NewOptions);
		InteractableObjectsChanged.Broadcast(CurrentOptions);
	}
}
