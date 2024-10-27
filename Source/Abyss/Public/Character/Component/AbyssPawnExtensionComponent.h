// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AbyssPawnData.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "AbyssPawnExtensionComponent.generated.h"


class UAbyssAbilitySystemComponent;
class UAbyssPawnData;

UCLASS()
class ABYSS_API UAbyssPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:

	UAbyssPawnExtensionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = "Abyss|Pawn")
	static UAbyssPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UAbyssPawnExtensionComponent>() : nullptr); }

	template<class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UAbyssPawnData* InPawnData);

	UFUNCTION(BlueprintPure, Category = "Abyss|Pawn")
	UAbyssAbilitySystemComponent* GetAbyssAbilitySystemComponent() const{ return AbilitySystemComponent; }

	void InitializeAbilitySystem(UAbyssAbilitySystemComponent* InASC, AActor* InOwnerActor);

	void UninitializeAbilitySystem();

	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate&& Delegate);

	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate&& Delegate);


	static const FName NAME_ActorFeatureName;

	//~IGameFrameworkInitStateInterface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~End of IGameFrameworkInitStateInterface

	void HandleControllerChanged();

	void SetupPlayerInputComponent();

private:

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:

	UPROPERTY(EditInstanceOnly, Category = "Abyss|Pawn")
	TObjectPtr<const UAbyssPawnData> PawnData;

	UPROPERTY(Transient)
	TObjectPtr<UAbyssAbilitySystemComponent> AbilitySystemComponent;

	FSimpleMulticastDelegate OnAbilitySystemInitialized;

	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
};
