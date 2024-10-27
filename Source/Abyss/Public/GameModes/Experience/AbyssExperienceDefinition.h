// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbyssExperienceDefinition.generated.h"

class UAbyssExperienceActionSet;
class UGameFeatureAction;
class UAbyssPawnData;
/**
 * 
 */
UCLASS(BlueprintType, Const)
class UAbyssExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UAbyssExperienceDefinition();

	// List of Game Feature Plugins this Experience wants to have active
	UPROPERTY(EditDefaultsOnly,Category=Gameplay)
	TArray<FString> GameFeaturesToEnable;

	//To Spawn For Player
	UPROPERTY(EditDefaultsOnly,Category=Gameplay)
	TObjectPtr<const UAbyssPawnData> DefaultPawnData;
	
	UPROPERTY(EditDefaultsOnly,Instanced,Category="Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	UPROPERTY(EditDefaultsOnly,Category=Gameplay)
	TArray<TObjectPtr<UAbyssExperienceActionSet>> ActionSets;
	
};
