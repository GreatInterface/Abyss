// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AbyssPlayerController.h"
#include "AbilitySystem/System/AbyssAbilitySystemComponent.h"
#include "Player/AbyssPlayerState.h"

AAbyssPlayerController::AAbyssPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

AAbyssPlayerState* AAbyssPlayerController::GetAbyssPlayerState() const
{
	return CastChecked<AAbyssPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UAbyssAbilitySystemComponent* AAbyssPlayerController::GetAbyssAbilitySystemComponent() const
{
	AAbyssPlayerState* AbyssPS = GetAbyssPlayerState();
	return AbyssPS ? AbyssPS->GetAbyssAbilitySystemComponent() : nullptr;
}

void AAbyssPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
}

void AAbyssPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);
}

void AAbyssPlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void AAbyssPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);

	if(UAbyssAbilitySystemComponent* AbyssASC = GetAbyssAbilitySystemComponent())
	{
		AbyssASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
}
