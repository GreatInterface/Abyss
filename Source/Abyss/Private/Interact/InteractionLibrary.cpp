// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/InteractionLibrary.h"
#include "Interact/InteractableTarget.h"
#include <functional>
#include "Engine/OverlapResult.h"

UInteractionLibrary::UInteractionLibrary()
	: Super(FObjectInitializer::Get())
{
}

AActor* UInteractionLibrary::GetActorFromInteractableTarget(TScriptInterface<IInteractableTarget> InteractableTarget)
{
	if(UObject* Object = InteractableTarget.GetObject())
	{
		if(AActor* Actor = Cast<AActor>(Object))
		{
			return Actor;
		}

		if(UActorComponent* ActorComponent = Cast<UActorComponent>(Object))
		{
			return ActorComponent->GetOwner();
		}

		unimplemented();
	}

	return nullptr;
}

void UInteractionLibrary::AppendInteractableTargetsFromOverlapResults(const TArray<FOverlapResult>& OverlapResults,
                                                                      OUT TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets)
{
	OutInteractableTargets.Reserve(OverlapResults.Num());
	for(const FOverlapResult& Single : OverlapResults)
	{
		if(TScriptInterface<IInteractableTarget> InteractableActor(Single.GetActor()); InteractableActor)
		{
			OutInteractableTargets.AddUnique(InteractableActor);
		}

		if(TScriptInterface<IInteractableTarget> InteractableComponent(Single.GetComponent()); InteractableComponent)
		{
			OutInteractableTargets.AddUnique(InteractableComponent);
		}
	}
}

void UInteractionLibrary::AppendInteractableTargetsFromHitResult(const FHitResult& HitResult,
	TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets)
{
	TScriptInterface<IInteractableTarget> InteractableActor(HitResult.GetActor());
	if (InteractableActor)
	{
		OutInteractableTargets.AddUnique(InteractableActor);
	}

	TScriptInterface<IInteractableTarget> InteractableComp(HitResult.GetComponent());
	if (InteractableComp)
	{
		OutInteractableTargets.AddUnique(InteractableComp);
	}
}

