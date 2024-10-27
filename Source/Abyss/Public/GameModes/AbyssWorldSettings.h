// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "AbyssWorldSettings.generated.h"

class UAbyssExperienceDefinition;
/**
 * 
 */
UCLASS()
class ABYSS_API AAbyssWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
	
public:

	AAbyssWorldSettings(const FObjectInitializer& ObjectInitializer);

	FPrimaryAssetId GetDefaultGameplayExperience() const; 

private:

	UPROPERTY(EditDefaultsOnly,Category=GameMode)
	TSoftClassPtr<UAbyssExperienceDefinition> DefaultGameplayExperience;
	
};
