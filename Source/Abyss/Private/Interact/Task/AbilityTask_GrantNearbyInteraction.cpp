// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/Task/AbilityTask_GrantNearbyInteraction.h"

#include "AbilitySystemComponent.h"
#include "Abyss/AbyssLogChannels.h"
#include "Interact/InteractableTarget.h"
#include "Interact/InteractionOption.h"
#include "Interact/InteractionQuery.h"
#include "Interact/InteractionLibrary.h"
#include "Physics/AbyssCollisionChannels.h"

UAbilityTask_GrantNearbyInteraction::UAbilityTask_GrantNearbyInteraction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAbilityTask_GrantNearbyInteraction::Activate()
{
	SetWaitingOnAvatar();

	UWorld* World = GetWorld();
	ensure(World);
	UE_LOG(LogAbyss, Verbose, TEXT("GrantNearbyInteraction Activate"));
	World->GetTimerManager().SetTimer(QueryTimerHandle, this, &ThisClass::QueryInteractables, InteractionScanRate, true);
}

UAbilityTask_GrantNearbyInteraction* UAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractors(
	UGameplayAbility* OwningAbility, float InteractionScanRange, float InteractionScanRate)
{
	UAbilityTask_GrantNearbyInteraction* MyObj = NewAbilityTask<UAbilityTask_GrantNearbyInteraction>(OwningAbility);
	MyObj->InteractionScanRange = InteractionScanRange;
	MyObj->InteractionScanRate = InteractionScanRate;

	return MyObj;
}

void UAbilityTask_GrantNearbyInteraction::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_GrantNearbyInteraction::QueryInteractables()
{
	UWorld* World = GetWorld();
	AActor* ActorOwner = GetAvatarActor();

	if(World && ActorOwner)
	{
		FCollisionQueryParams Params(SCENE_QUERY_STAT(UAbilityTask_GrantNearbyInteraction), false);
		
		TArray<FOverlapResult> OverlapResults;
		World->OverlapMultiByChannel(OUT OverlapResults, ActorOwner->GetActorLocation(), FQuat::Identity, Abyss_TraceChannel_Interaction, FCollisionShape::MakeSphere(InteractionScanRange), Params);

		if(OverlapResults.Num() > 0)
		{
			//找到检测球内所有实现IInteractableTarget接口的对象(可交互目标)
			TArray<TScriptInterface<IInteractableTarget>> InteractableTargets;
			UInteractionLibrary::AppendInteractableTargetsFromOverlapResults(OverlapResults, OUT InteractableTargets);

			FInteractionQuery InteractionQuery;
			InteractionQuery.RequestingAvatar = ActorOwner;
			InteractionQuery.RequestingController = Cast<AController>(ActorOwner->GetOwner());

			//收集每个 可交互目标 的 可交互选项并存储在Options数组中
			TArray<FInteractionOption> Options;
			for(auto& Target : InteractableTargets)
			{
				FInteractionOptionBuilder OptionBuilder(Target, Options);
				Target->GatherInteractionOptions(InteractionQuery, OUT OptionBuilder);
			}

			for(FInteractionOption& Entry : Options)
			{
				if(Entry.InteractionAbilityToGrant)
				{
					FObjectKey ObjectKey(Entry.InteractionAbilityToGrant);
					if(!InteractionAbilityCache.Find(ObjectKey))
					{
						FGameplayAbilitySpec Spec(Entry.InteractionAbilityToGrant, 1, INDEX_NONE, this);
						FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
						InteractionAbilityCache.Add(ObjectKey, Handle);
					}
				}
			}
		}
	}
}
