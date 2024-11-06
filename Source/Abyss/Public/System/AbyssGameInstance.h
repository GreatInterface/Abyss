// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonGameInstance.h"
#include "Engine/GameInstance.h"
#include "AbyssGameInstance.generated.h"

class AAbyssPlayerController;
/**
 * 
 */
UCLASS(Config=Game)
class ABYSS_API UAbyssGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

public:

	UAbyssGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	AAbyssPlayerController* GetPrimaryPlayerController() const;
	
protected:

	virtual void Init() override;
};
