// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ModularGameMode.h"
#include "AbyssGameMode.generated.h"

class UAbyssPawnData;
class UAbyssExperienceDefinition;
/**
 * 
 */
UCLASS(Config = Game, meta = (ShortTooltip = "The base game mode class used by this project."))
class ABYSS_API AAbyssGameMode : public AModularGameModeBase
{
	GENERATED_BODY()

public:
	
	AAbyssGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~Begin AGameModeBase interface
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
	//~End AGameModeBase interface

	UFUNCTION(BlueprintCallable,Category="Abyss|Pawn")
	const UAbyssPawnData* GetPawnDataForController(const AController* InController);

private:

	void HandleMatchAssignmentIfNotExperienceOne();
	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId,const FString& ExperienceIdSource);

	void OnExperienceLoaded(const UAbyssExperienceDefinition* CurrentExperience);
	bool IsExperienceLoaded() const;
};


