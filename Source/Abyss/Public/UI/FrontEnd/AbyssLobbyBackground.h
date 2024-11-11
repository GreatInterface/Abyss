// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbyssLobbyBackground.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssLobbyBackground : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lobby")
	TSoftObjectPtr<UWorld> BackgroundLevel;
};
