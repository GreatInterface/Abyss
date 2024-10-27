// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AbyssGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UAbyssGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void Init() override;
};
