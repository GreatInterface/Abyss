// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AbyssLocalPlayer.h"

UAbyssLocalPlayer::UAbyssLocalPlayer()
{
}

bool UAbyssLocalPlayer::SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld)
{
	return Super::SpawnPlayActor(URL, OutError, InWorld);
}
