// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbyssPawnData.generated.h"

class UAbyssAbilitySet;
class UAbyssAbilityTagRelationshipMapping;
class UAbyssInputConfig;
/**
 * 
 */
UCLASS(BlueprintType, Const, meta=(DisplayName = "Abyss Pawn Data", ShortTooltip = "Data asset used to define a Pawn"))
class ABYSS_API UAbyssPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UAbyssPawnData(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abyss|Pawn")
	TSubclassOf<APawn> PawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abyss|Input")
	TObjectPtr<UAbyssInputConfig> InputConfig; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Abyss|Ability")
	TObjectPtr<UAbyssAbilityTagRelationshipMapping> TagRelationshipMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Abyss|Ability")
	TArray<TObjectPtr<UAbyssAbilitySet>> AbilitySets;
};
