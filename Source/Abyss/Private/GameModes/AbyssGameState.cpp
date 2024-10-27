// Fill out your copyright notice in the Description page of Project Settings.


#include "Abyss/Public/GameModes/AbyssGameState.h"
#include "AbilitySystem/System/AbyssAbilitySystemComponent.h"
#include "GameModes/Experience/AbyssExperienceManagerComponent.h"

AAbyssGameState::AAbyssGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UAbyssAbilitySystemComponent>(this,"AbilitySystemComponent");
	
	ExperienceManagerComponent = CreateDefaultSubobject<UAbyssExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}

void AAbyssGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AAbyssGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent)
	AbilitySystemComponent->InitAbilityActorInfo(/*Owner*/ this,/*Avatar*/ this);
}

UAbilitySystemComponent* AAbyssGameState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
