// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/System/AbyssGameplayCueManager.h"

#include "AbilitySystemGlobals.h"

UAbyssGameplayCueManager::UAbyssGameplayCueManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAbyssGameplayCueManager* UAbyssGameplayCueManager::Get()
{
	return Cast<UAbyssGameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager());
}
