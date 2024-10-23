// Fill out your copyright notice in the Description page of Project Settings.


#include "Abyss/Public/GameModes/AbyssGameMode.h"

#include "Abyss/AbyssLogChannels.h"
#include "Abyss/Public/GameModes/AbyssWorldSettings.h"
#include "Character/AbyssCharacter.h"
#include "Character/Component/AbyssPawnExtensionComponent.h"
#include "GameModes/AbyssGameState.h"
#include "GameModes/Experience/AbyssExperienceActionSet.h"
#include "GameModes/Experience/AbyssExperienceDefinition.h"
#include "GameModes/Experience/AbyssExperienceManagerComponent.h"
#include "Player/AbyssPlayerController.h"
#include "Player/AbyssPlayerState.h"

AAbyssGameMode::AAbyssGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GameStateClass = AAbyssGameState::StaticClass();
	PlayerControllerClass = AAbyssPlayerController::StaticClass();
	PlayerStateClass = AAbyssPlayerState::StaticClass();
	DefaultPawnClass = AAbyssCharacter::StaticClass();
}

void AAbyssGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	GetWorld()->GetTimerManager().SetTimerForNextTick(this,&ThisClass::HandleMatchAssignmentIfNotExperienceOne);
}

void AAbyssGameMode::HandleMatchAssignmentIfNotExperienceOne()
{
	FPrimaryAssetId ExperienceId;
	FString ExperienceIdSource;

	// Precedence order(highest wins)
	// - World Settings
	// - Default Experience

	UWorld* World = GetWorld();

	//World Settings
	if(!ExperienceId.IsValid())
	{
		if(AAbyssWorldSettings* TypesWorldSettings = Cast<AAbyssWorldSettings>(GetWorldSettings()))
		{
			ExperienceId = TypesWorldSettings->GetDefaultGameplayExperience();
			ExperienceIdSource = TEXT("WorldSettings");
		}
	}

	//Default Experience
	if(!ExperienceId.IsValid())
	{
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("AbyssExperienceDefinition"), FName("B_AbyssDefaultExperience"));
		ExperienceIdSource = TEXT("Default");
	}

	OnMatchAssignmentGiven(ExperienceId,ExperienceIdSource);
}

void AAbyssGameMode::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
	if(ExperienceId.IsValid())
	{
		UAbyssExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UAbyssExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->SetCurrentExperience(ExperienceId);
	}
	else
	{
		UE_LOG(LogAbyssExperience,Error,TEXT("Failed to identify experience, loading screen will stay up forever"));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
///
void AAbyssGameMode::InitGameState()
{
	Super::InitGameState();

	check(GameState)
	UAbyssExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UAbyssExperienceManagerComponent>();
	check(ExperienceComponent)
	
	ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnAbyssExperienceLoaded::FDelegate::CreateUObject(this,&ThisClass::OnExperienceLoaded));
}

void AAbyssGameMode::OnExperienceLoaded(const UAbyssExperienceDefinition* CurrentExperience)
{
	//Spawn any players that are already attached
	//无论如何，GetDefaultPawnClassForController_Implementation 可能只会为玩家调用
	for(FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = Cast<APlayerController>(*It);
		if((PC) && (PC->GetPawn() == nullptr))
		{
			if(PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

UClass* AAbyssGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if(const UAbyssPawnData* PawnData = GetPawnDataForController(InController))
	{
		if(PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}
	
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* AAbyssGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,
	const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	if(UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if(APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass,SpawnTransform,SpawnInfo))
		{
			if(	UAbyssPawnExtensionComponent* PawnExtComponent = SpawnedPawn->FindComponentByClass<UAbyssPawnExtensionComponent>())
			{
				if(const UAbyssPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComponent->SetPawnData(PawnData);
				}	
			}
			
			SpawnedPawn->FinishSpawning(SpawnTransform);
			
			return SpawnedPawn;
		}
	}
	
	return nullptr;
}

void AAbyssGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if(IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

AActor* AAbyssGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	//TODO : PlayerSpawningComponent
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

void AAbyssGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	//TODO : PlayerSpawningComponent
	
	Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool AAbyssGameMode::IsExperienceLoaded() const
{
	check(GameState)
	UAbyssExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UAbyssExperienceManagerComponent>();
	if(ExperienceComponent)
	{
		return ExperienceComponent->IsExperienceLoaded();
	}

	return false;
}

const UAbyssPawnData* AAbyssGameMode::GetPawnDataForController(const AController* InController)
{
	if(InController != nullptr)
	{
		if(const AAbyssPlayerState* AbyssPS = InController->GetPlayerState<AAbyssPlayerState>())
		{
			if(const UAbyssPawnData* PawnData = AbyssPS->GetPawnData<UAbyssPawnData>())
			{
				return PawnData;
			}
		}
	}

	check(GameState)
	UAbyssExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UAbyssExperienceManagerComponent>();
	check(ExperienceComponent)

	if(ExperienceComponent->IsExperienceLoaded())
	{
		const UAbyssExperienceDefinition* Experience = ExperienceComponent->GetCurrentExperienceChecked();
		if(Experience->DefaultPawnData)
		{
			return Experience->DefaultPawnData;
		}
	}

	return nullptr;
}
