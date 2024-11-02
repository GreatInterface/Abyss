// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AbyssHUD.generated.h"


UCLASS(Config=Game)
class ABYSS_API AAbyssHUD : public AHUD
{
	GENERATED_BODY()
public:
	
	AAbyssHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	//~UObject
	virtual void PreInitializeComponents() override;

	//~Actor
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//~AHUD
	virtual void GetDebugActorList(TArray<AActor*>& InOutList) override;
};
