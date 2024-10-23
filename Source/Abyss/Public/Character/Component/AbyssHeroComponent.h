// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "AbyssHeroComponent.generated.h"


class UAbyssInputConfig;
struct FInputActionValue;
struct FInputMappingContextAndPriority;

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class ABYSS_API UAbyssHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:

	UAbyssHeroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Abyss|Hero")
	static UAbyssHeroComponent* FindHeroComponent(const AActor* Actor) { return Actor ? Actor->FindComponentByClass<UAbyssHeroComponent>() : nullptr; }

private:

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	static const FName NAME_ActorFeatureName;

	//~IGameFrameworkInitStateInterface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~End of IGameFrameworkInitStateInterface

	//~Input
	void AddAdditionalInputConfig(const UAbyssInputConfig* InputConfig, TArray<uint32>& Handles);
	void RemoveAdditionalInputConfigByHandles(TArray<uint32>& Handles);
	//~End of Input
	
private:

	void InitializePlayerInput(UInputComponent* PlayerInputComponent);
	
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookStick(const FInputActionValue& InputActionValue);
	void Input_Crouch(const FInputActionValue& InputActionValue);
	void Input_AutoRun(const FInputActionValue& InputActionValue);

	UPROPERTY(EditAnywhere)
	TArray<FInputMappingContextAndPriority> DefaultInputMappings;

	bool bReadyToBindInputs;

public:
	static const FName NAME_BindInputsNow;

	[[nodiscard]] bool IsReadyToBindInputs() const { return bReadyToBindInputs; }
};
