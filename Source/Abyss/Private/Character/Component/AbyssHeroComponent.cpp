// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/AbyssHeroComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "AbilitySystem/System/AbyssAbilitySystemComponent.h"
#include "Abyss/AbyssGameplayTags.h"
#include "Abyss/AbyssLogChannels.h"
#include "Character/AbyssCharacter.h"
#include "Character/Component/AbyssPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFeatures/GameFeatureAction_AddInputContextMapping.h"
#include "Input/AbyssInputComponent.h"
#include "Player/AbyssLocalPlayer.h"
#include "Player/AbyssPlayerController.h"
#include "Player/AbyssPlayerState.h"
#include "UserSettings/EnhancedInputUserSettings.h"

namespace AbyssHero
{
	static const float LookYawRate = 300.f;
	static const float LookPitchRate = 165.0f;
}

const FName UAbyssHeroComponent::NAME_ActorFeatureName(TEXT("Hero"));
const FName UAbyssHeroComponent::NAME_BindInputsNow("Hero");

UAbyssHeroComponent::UAbyssHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReadyToBindInputs = false;
}

void UAbyssHeroComponent::OnRegister()
{
	Super::OnRegister();

	if(GetPawn<APawn>())
	{
		RegisterInitStateFeature();
	}
}

void UAbyssHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(UAbyssPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	ensure(TryToChangeInitState(AbyssGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UAbyssHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	
	Super::EndPlay(EndPlayReason);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool UAbyssHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState) const
{
	check(Manager)

	APawn* Pawn = GetPawn<APawn>();
	check(Pawn);
	
	if (GetPlayerState<AAbyssPlayerState>())
	{
		UE_LOG(LogAbyss, Display, TEXT("PlayerState"));
	}
	
	if(!CurrentState.IsValid() && DesiredState == AbyssGameplayTags::InitState_Spawned)
	{
		//判定组件已经依附在Pawn上
		if(Pawn)
		{
			return true;
		}
	}
	if(CurrentState == AbyssGameplayTags::InitState_Spawned && DesiredState == AbyssGameplayTags::InitState_DataAvailable)
	{
		if(!GetPlayerState<AAbyssPlayerState>())
		{
			return false;
		}
		if(!Pawn->InputComponent || !GetController<AAbyssPlayerController>() || !Pawn->IsLocallyControlled())
		{
			return false;
		}

		return true;
	}
	if(CurrentState == AbyssGameplayTags::InitState_DataAvailable && DesiredState == AbyssGameplayTags::InitState_DataInitialized)
	{
		return GetPlayerState<AAbyssPlayerState>() &&
			Manager->HasFeatureReachedInitState(Pawn,UAbyssPawnExtensionComponent::NAME_ActorFeatureName,AbyssGameplayTags::InitState_DataInitialized);
	}
	if(CurrentState == AbyssGameplayTags::InitState_DataInitialized && DesiredState == AbyssGameplayTags::InitState_GameplayReady)
	{
		return true;
	}
	
	return false;
}

void UAbyssHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState)
{
	UE_LOG(LogAbyss, Log, TEXT(" Current[%s],DesiredState[%s]"),*CurrentState.ToString(),*DesiredState.ToString());
	
	if(CurrentState == AbyssGameplayTags::InitState_DataAvailable && DesiredState == AbyssGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		AAbyssPlayerState* AbyssPS = GetPlayerState<AAbyssPlayerState>();
		ensure(Pawn && AbyssPS);
		
		if(UAbyssPawnExtensionComponent* PawnExtComp = UAbyssPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnExtComp->InitializeAbilitySystem(AbyssPS->GetAbyssAbilitySystemComponent(), AbyssPS);
		}

		if(AAbyssPlayerController* AbyssPC = GetController<AAbyssPlayerController>())
		{
			if(Pawn->InputComponent)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}
	}
}

void UAbyssHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if(Params.FeatureName == UAbyssPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if(Params.FeatureState == AbyssGameplayTags::InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

void UAbyssHeroComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = { AbyssGameplayTags::InitState_Spawned, AbyssGameplayTags::InitState_DataAvailable, AbyssGameplayTags::InitState_DataInitialized, AbyssGameplayTags::InitState_GameplayReady };

	ContinueInitStateChain(StateChain);
}

/////////////////////////////////////////////////////////////////////////////////////////

void UAbyssHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent)

	const APawn* Pawn = GetPawn<APawn>();
	check(Pawn)

	const APlayerController* PC = GetController<APlayerController>();
	check(PC)
	
	const UAbyssLocalPlayer* LP = Cast<UAbyssLocalPlayer>(PC->GetLocalPlayer());
	check(LP)

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem)

	Subsystem->ClearAllMappings();

	//之所以使用PawnExtComp的PawnData，而不是PS的，是因为我们可能操控不同的角色或载具
	if(UAbyssPawnExtensionComponent* PawnExtComp = UAbyssPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if(const UAbyssPawnData* PawnData = PawnExtComp->GetPawnData<UAbyssPawnData>())
		{
			if(const UAbyssInputConfig* Config = PawnData->InputConfig)
			{
				for(const FInputMappingContextAndPriority& CP : DefaultInputMappings)
				{
					if(UInputMappingContext* IMC = CP.InputMapping.Get())
					{
						if(CP.bRegisterWithSettings)
						{
							if(UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
							{
								Settings->RegisterInputMappingContext(IMC);
							}

							FModifyContextOptions Options {};
							Options.bIgnoreAllPressedKeysUntilRelease = false;
							//Actually add the config to the local player
							Subsystem->AddMappingContext(IMC, CP.Priority, Options);
						}
					}
				}

				UAbyssInputComponent* AbyssIC = Cast<UAbyssInputComponent>(PlayerInputComponent);
				if(ensure(AbyssIC))
				{
					AbyssIC->AddInputMapping(Config, Subsystem);
					
					TArray<uint32> BindHandles;
					AbyssIC->BindAbilityActions(Config, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, BindHandles);

					AbyssIC->BindNativeAction(Config, AbyssGameplayTags::InputTag_Move,		ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
					AbyssIC->BindNativeAction(Config, AbyssGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, false);
					AbyssIC->BindNativeAction(Config, AbyssGameplayTags::InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, false);
					AbyssIC->BindNativeAction(Config, AbyssGameplayTags::InputTag_Crouch,		ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch,	false);
					AbyssIC->BindNativeAction(Config, AbyssGameplayTags::InputTag_AutoRun,	ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, false);
				}
			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn),			 NAME_BindInputsNow);
}

void UAbyssHeroComponent::AddAdditionalInputConfig(const UAbyssInputConfig* InputConfig, TArray<uint32>& Handles)
{
	const APawn* Pawn = GetPawn<APawn>();
	if(!Pawn) return;

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const UAbyssPawnExtensionComponent* PawnExtComp = UAbyssPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		UAbyssInputComponent* AbyssIC = Pawn->FindComponentByClass<UAbyssInputComponent>();
		if (ensure(AbyssIC))
		{
			AbyssIC->AddInputMapping(InputConfig, Subsystem);
			
			AbyssIC->BindAbilityActions(InputConfig, this,
				&ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, Handles);
		}
	}
}

void UAbyssHeroComponent::RemoveAdditionalInputConfigByHandles(TArray<uint32>& Handles)
{
	const APawn* Pawn = GetPawn<APawn>();
	if(!Pawn) return;
	
	const APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (!PC)
	{
		UE_LOG(LogAbyss, Warning, TEXT("UAbyssHeroComponent Can't Find Player Controller"));
		return;
	}
	
	if (UAbyssInputComponent* AbyssIC = Cast<UAbyssInputComponent>(Pawn->InputComponent))
	{
		if (ensure(AbyssIC))
		{
			AbyssIC->RemoveAbilityActionsByHandles(Handles);
			Handles.Empty();
		}
	}
}

void UAbyssHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if(const APawn* Pawn = GetPawn<APawn>())
	{
		if(UAbyssPawnExtensionComponent* PawnExtComp = UAbyssPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if(UAbyssAbilitySystemComponent* AbyssASC = PawnExtComp->GetAbyssAbilitySystemComponent())
			{
				AbyssASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

void UAbyssHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if(const APawn* Pawn = GetPawn<APawn>())
	{
		if(UAbyssPawnExtensionComponent* PawnExtComp = UAbyssPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if(UAbyssAbilitySystemComponent* AbyssASC = PawnExtComp->GetAbyssAbilitySystemComponent())
			{
				AbyssASC->AbilityInputTagReleased(InputTag);
			}
		}
	}
}

void UAbyssHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if(AAbyssPlayerController* AbyssPC = Cast<AAbyssPlayerController>(Controller))
	{
		//TODO : SetIsAutoRunning(false);
	}

	if(Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0, Controller->GetControlRotation().Yaw, 0.0);
		
		if(Value.X != 0.f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if(Value.Y != 0.f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UAbyssHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	if(APawn* Pawn = GetPawn<APawn>())
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();

		if(Value.X != 0.f)
		{
			Pawn->AddControllerYawInput(Value.X);
		}

		if(Value.Y != 0.f)
		{
			Pawn->AddControllerPitchInput(Value.Y);
		}
	}
}

void UAbyssHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	if(APawn* Pawn = GetPawn<APawn>())
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();

		UWorld* World = GetWorld();
		check(World)
		
		if(Value.X != 0.f)
		{
			Pawn->AddControllerYawInput(Value.X * AbyssHero::LookYawRate * World->GetDeltaSeconds());
		}

		if(Value.Y != 0.f)
		{
			Pawn->AddControllerPitchInput(Value.Y * AbyssHero::LookPitchRate * World->GetDeltaSeconds());
		}
	}
}

void UAbyssHeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)
{
	if(AAbyssCharacter* Character = GetPawn<AAbyssCharacter>())
	{
		Character->ToggleCrouch();
	}
}

void UAbyssHeroComponent::Input_AutoRun(const FInputActionValue& InputActionValue)
{
	//TODO
}
