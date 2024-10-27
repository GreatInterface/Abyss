// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AbyssPlayerState.h"
#include "AbilitySystem/AbyssAbilitySet.h"
#include "AbilitySystem/System/AbyssAbilitySystemComponent.h"
#include "Abyss/AbyssLogChannels.h"
#include "GameModes/AbyssGameMode.h"
#include "Abyss/Public/Character/AbyssPawnData.h"
#include "Character/Component/AbyssPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameModes/Experience/AbyssExperienceDefinition.h"
#include "GameModes/Experience/AbyssExperienceManagerComponent.h"
#include "Player/AbyssPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbyssPlayerState)

const FName AAbyssPlayerState::NAME_AbyssAbilityReady("AbyssAbilitiesReady");

AAbyssPlayerState::AAbyssPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UAbyssAbilitySystemComponent>(this,"AbilitySystemComponent");
}

AAbyssPlayerController* AAbyssPlayerState::GetAbyssPlayerController() const
{
	return Cast<AAbyssPlayerController>(GetOwner());
}

void AAbyssPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AAbyssPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent)
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	UWorld* World = GetWorld();
	if(World && World->IsGameWorld())
	{
		AGameStateBase* GameState = World->GetGameState();
		check(GameState)
		UAbyssExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UAbyssExperienceManagerComponent>();
		check(ExperienceComponent)
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnAbyssExperienceLoaded::FDelegate::CreateUObject(this,&ThisClass::OnExperienceLoaded));
	}

	if(UAbyssPawnExtensionComponent* ExtComp = UAbyssPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	{
		ExtComp->CheckDefaultInitialization();
	}
}

void AAbyssPlayerState::OnExperienceLoaded(const UAbyssExperienceDefinition* CurrentExperience)
{
	if(CurrentExperience && CurrentExperience->DefaultPawnData)
	{
		SetPawnData(CurrentExperience->DefaultPawnData);
	}
	if(AAbyssGameMode* GameMode = GetWorld()->GetAuthGameMode<AAbyssGameMode>())
	{
		if(const UAbyssPawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
		else
		{
			UE_LOG(LogAbyss, Error, TEXT("ALyraPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
		}
	}
}

void AAbyssPlayerState::SetPawnData(const UAbyssPawnData* InPawnData)
{
	check(InPawnData)
	if(PawnData)
	{
		return;
	}

	PawnData = InPawnData;

	for(const auto& AbilitySet : PawnData->AbilitySets)
	{
		if(AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_AbyssAbilityReady);
}

UAbilitySystemComponent* AAbyssPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
