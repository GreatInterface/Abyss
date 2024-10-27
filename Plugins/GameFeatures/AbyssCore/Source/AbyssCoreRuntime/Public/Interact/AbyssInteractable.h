// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interact/InteractableTarget.h"
#include "AbyssInteractable.generated.h"

UCLASS(Abstract, Blueprintable)
class ABYSSCORERUNTIME_API AAbyssInteractable : public AActor, public IInteractableTarget
{
	GENERATED_BODY()
	
public:	
	
	AAbyssInteractable();

	/** 收集可交互选项 */
	virtual void GatherInteractionOptions(const FInteractionQuery& InteractionQuery, FInteractionOptionBuilder& OptionBuilder) override;
	/** 定制EventData */
	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) override;

protected:
	UPROPERTY(EditAnywhere, Category="Abyss|Interact")
	FInteractionOption InteractOption;
};
