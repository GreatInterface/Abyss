// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemInterface.h"
#include "ModularCharacter.h"
#include "AbyssCharacter.generated.h"

class UAbyssCameraComponent;
class UAbyssSpringArmComponent;
class UAbyssPawnExtensionComponent;
class UAbyssAbilitySystemComponent;
class AAbyssPlayerState;
class AAbyssPlayerController;

UCLASS(Config = Game)
class ABYSS_API AAbyssCharacter : public AModularCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AAbyssCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Abyss|Character")
	AAbyssPlayerController* GetAbyssPlayerController() const;

	UFUNCTION(BlueprintCallable,Category = "Abyss|Character")
	AAbyssPlayerState* GetAbyssPlayerState() const;

	UFUNCTION(BlueprintCallable,Category = "Abyss|Character")
	UAbyssAbilitySystemComponent* GetAbyssAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void ToggleCrouch();

private:

	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abyss|Character", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbyssPawnExtensionComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAbyssSpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAbyssCameraComponent> CameraComponent;
};
