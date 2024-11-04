// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameUIManagerSubsystem.h"
#include "AbyssUIManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssUIManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()

public:

	UAbyssUIManagerSubsystem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	bool Tick(float DeltaTime);
	void SyncRootLayoutVisibilityToShowHUD();

	FTSTicker::FDelegateHandle TickHandle;
};
