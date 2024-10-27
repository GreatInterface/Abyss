// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayMessageSubsystem.generated.h"

UCLASS()
class GAMEPLAYMESSAGERUNTIME_API UGameplayMessageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	static UGameplayMessageSubsystem& Get(const UObject* WorldContextObject);

	/**
	 * @return true if 在当前World中存在一个已经激活的GameplayMessageSubsystem
	 */
	static bool HasInstance(const UObject* WorldContextObject);
};
