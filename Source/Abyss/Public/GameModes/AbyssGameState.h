// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "AbilitySystemInterface.h"
#include "ModularGameState.h"
#include "AbyssGameState.generated.h"

class UAbyssAbilitySystemComponent;
class UAbyssExperienceManagerComponent;
/**
 * 
 */
UCLASS(Config = Game)
class ABYSS_API AAbyssGameState : public AModularGameStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AAbyssGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor

	//~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface

	UAbyssAbilitySystemComponent* GetAbyssAbilitySystemComponent() const {return AbilitySystemComponent;}
	
private:

	UPROPERTY()
	TObjectPtr<UAbyssExperienceManagerComponent> ExperienceManagerComponent;

	//主要用于Gameplay Cue
	UPROPERTY(VisibleAnywhere,Category="Abyss|GameState")
	TObjectPtr<UAbyssAbilitySystemComponent> AbilitySystemComponent;
};
