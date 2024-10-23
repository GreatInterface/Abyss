// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CommonPlayerController.h"
#include "AbyssPlayerController.generated.h"

class UAbyssAbilitySystemComponent;
class AAbyssPlayerState;
/**
 * 
 */
UCLASS()
class ABYSS_API AAbyssPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()

public:

	AAbyssPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Abyss|PlayerController")
	AAbyssPlayerState* GetAbyssPlayerState() const;

	UFUNCTION(BlueprintCallable, Category="Abyss|PlayerController")
	UAbyssAbilitySystemComponent* GetAbyssAbilitySystemComponent() const;
	
	//~ AController interface
	virtual void InitPlayerState() override;
	//~ End of AController interface

	//~ APlayerController interface
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~ End of APlayerController interface

};
