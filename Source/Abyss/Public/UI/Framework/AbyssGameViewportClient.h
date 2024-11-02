// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonGameViewportClient.h"
#include "AbyssGameViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssGameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()

public:
	UAbyssGameViewportClient(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice) override;
};
