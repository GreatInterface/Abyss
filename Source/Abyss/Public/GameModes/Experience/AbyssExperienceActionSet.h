// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbyssExperienceActionSet.generated.h"

class UGameFeatureAction;
/**
 * 
 */
UCLASS(BlueprintType,NotBlueprintable)
class ABYSS_API UAbyssExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UAbyssExperienceActionSet();

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif
	
	UPROPERTY(EditDefaultsOnly,Instanced,Category="Actions to Perform")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	UPROPERTY(EditAnywhere,Category="Feature Dependencies")
	TArray<FString> GameFeatureToEnable;
};
