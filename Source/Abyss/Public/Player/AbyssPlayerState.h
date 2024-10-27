// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "AbilitySystemInterface.h"
#include "Character/AbyssPawnData.h"
#include "ModularPlayerState.h"
#include "AbyssPlayerState.generated.h"

class AAbyssPlayerController;
class UAbyssAbilitySystemComponent;
class UAbyssPawnData;
class UAbyssExperienceDefinition;
/**
 * 
 */
UCLASS(Config = Game)
class ABYSS_API AAbyssPlayerState : public AModularPlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AAbyssPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable,Category="Abyss|PlayerState")
	AAbyssPlayerController* GetAbyssPlayerController() const;
	
	template<class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UAbyssPawnData* InPawnData);

	//~AActor
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor

	UFUNCTION(BlueprintCallable,Category="Abyss|PlayerState")
	UAbyssAbilitySystemComponent* GetAbyssAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	static const FName NAME_AbyssAbilityReady;
	
private:
	
	void OnExperienceLoaded(const UAbyssExperienceDefinition* CurrentExperience);

protected:

	UPROPERTY()
	TObjectPtr<const UAbyssPawnData> PawnData;

private:

	UPROPERTY(VisibleAnywhere,Category="Abyss|PlayerState")
	TObjectPtr<UAbyssAbilitySystemComponent> AbilitySystemComponent;
};
