// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatures/GameFeatureAction_AddInputContextMapping.h"
#include "EnhancedInputSubsystems.h"
#include "GameFeaturesSubsystem.h"
#include "InputMappingContext.h"
#include "Abyss/AbyssLogChannels.h"
#include "Character/Component/AbyssHeroComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "System/AbyssAssetManager.h"
#include "UserSettings/EnhancedInputUserSettings.h"

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureRegistering()
{
	Super::OnGameFeatureRegistering();

	RegisterInputMappingContexts();
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);
	if(!ActiveData.ExtensionRequestHandles.IsEmpty() || !ActiveData.ControllersAddedTo.IsEmpty())
	{
		Reset(ActiveData);
	}
	
	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	FPerContextData* Data = ContextData.Find(Context);
	if(ensure(Data))
	{
		Reset(*Data);
	}
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureUnregistering()
{
	Super::OnGameFeatureUnregistering();

	UnregisterInputMappingContexts();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///	~InputMappingContexts

void UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContexts()
{
	RegisterInputContextMappingsForGameInstanceHandle = FWorldDelegates::OnStartGameInstance.AddUObject(this, &ThisClass::RegisterInputMappingContextsForGameInstance);

	const auto& WorldContexts = GEngine->GetWorldContexts();
	for(auto WorldContextIt = WorldContexts.CreateConstIterator(); WorldContextIt; ++WorldContextIt)
	{
		RegisterInputMappingContextsForGameInstance(WorldContextIt->OwningGameInstance);
	}
}

void UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContextsForGameInstance(UGameInstance* GameInstance)
{
	if(GameInstance && !GameInstance->OnLocalPlayerAddedEvent.IsBoundToObject(this))
	{
		GameInstance->OnLocalPlayerAddedEvent.AddUObject(this, &ThisClass::RegisterInputMappingContextsForLocalPlayer);
		GameInstance->OnLocalPlayerRemovedEvent.AddUObject(this, &ThisClass::UnregisterInputMappingContextsForLocalPlayer);

		for(auto LocalPlayerIt = GameInstance->GetLocalPlayerIterator(); LocalPlayerIt; ++LocalPlayerIt)
		{
			RegisterInputMappingContextsForLocalPlayer(*LocalPlayerIt);
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)
{
	if(ensure(LocalPlayer))
	{
		UAbyssAssetManager& AssetManager = UAbyssAssetManager::Get();

		if(UEnhancedInputLocalPlayerSubsystem* EISubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if(UEnhancedInputUserSettings* Settings = EISubsystem->GetUserSettings())
			{
				for(const FInputMappingContextAndPriority& Entry : InputMappings)
				{
					if(!Entry.bRegisterWithSettings)
					{
						continue;
					}

					if(UInputMappingContext* IMC = AssetManager.GetAsset(Entry.InputMapping))
					{
						Settings->RegisterInputMappingContext(IMC);
						UE_LOG(LogGameFeatures, Display, TEXT("Success to Bind IMC %s"), *GetNameSafe(IMC));
					}
				}
			}
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContexts()
{
	FWorldDelegates::OnStartGameInstance.Remove(RegisterInputContextMappingsForGameInstanceHandle);
	RegisterInputContextMappingsForGameInstanceHandle.Reset();

	const auto& WorldContexts = GEngine->GetWorldContexts();
	for(auto WorldContextIt = WorldContexts.CreateConstIterator(); WorldContextIt; ++WorldContextIt)
	{
		UnregisterInputMappingContextsForGameInstance(WorldContextIt->OwningGameInstance);
	}
}

void UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContextsForGameInstance(
	UGameInstance* GameInstance)
{
	if(GameInstance)
	{
		GameInstance->OnLocalPlayerAddedEvent.RemoveAll(this);
		GameInstance->OnLocalPlayerRemovedEvent.RemoveAll(this);

		for(auto LocalPlayerIt = GameInstance->GetLocalPlayerIterator(); LocalPlayerIt; ++LocalPlayerIt)
		{
			UnregisterInputMappingContextsForLocalPlayer(*LocalPlayerIt);
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)
{
	ensure(LocalPlayer);

	if(UEnhancedInputLocalPlayerSubsystem* EISubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		if(UEnhancedInputUserSettings* Settings = EISubsystem->GetUserSettings())
		{
			for(const FInputMappingContextAndPriority& Entry : InputMappings)
			{
				if(!Entry.bRegisterWithSettings)
				{
					continue;
				}

				if(UInputMappingContext* IMC = Entry.InputMapping.Get())
				{
					Settings->UnregisterInputMappingContext(IMC);
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void UGameFeatureAction_AddInputContextMapping::AddToWorld(const FWorldContext& WorldContext,
	const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if((GameInstance) && (World) && (World->IsGameWorld()))
	{
		if(UGameFrameworkComponentManager* ComponentManager = GameInstance->GetSubsystem<UGameFrameworkComponentManager>())
		{
			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle =
				ComponentManager->AddExtensionHandler(
					APlayerController::StaticClass(),
					UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject
						(this, &ThisClass::HandleControllerExtension, ChangeContext));

			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///

void UGameFeatureAction_AddInputContextMapping::Reset(FPerContextData& ActiveData)
{
	ActiveData.ExtensionRequestHandles.Empty();

	while (!ActiveData.ControllersAddedTo.IsEmpty())
	{
		TWeakObjectPtr<APlayerController> ControllerWeakPtr = ActiveData.ControllersAddedTo.Top();
		if(ControllerWeakPtr.IsValid())
		{
			RemoveInputMappingAndController(ControllerWeakPtr.Get(), ActiveData);
		}
		else
		{
			ActiveData.ControllersAddedTo.Pop();
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::HandleControllerExtension(AActor* Actor, FName EventName,
	FGameFeatureStateChangeContext ChangeContext)
{
	APlayerController* AsController = CastChecked<APlayerController>(Actor);
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		RemoveInputMappingAndController(AsController, ActiveData);
	}
	if((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UAbyssHeroComponent::NAME_BindInputsNow))
	{
		AddInputMappingForPlayer(AsController->GetLocalPlayer(), ActiveData);
	}
}

void UGameFeatureAction_AddInputContextMapping::AddInputMappingForPlayer(UPlayer* Player,
	FPerContextData& ActiveData)
{
	if(ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
	{
		if(UEnhancedInputLocalPlayerSubsystem* EISubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			for(const FInputMappingContextAndPriority& Entry : InputMappings)
			{
				if(const UInputMappingContext* IMC = Entry.InputMapping.Get())
				{
					EISubsystem->AddMappingContext(IMC, Entry.Priority);
				}
			}
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::RemoveInputMappingAndController(APlayerController* PlayerController,
	FPerContextData& ActiveData)
{
	if(ULocalPlayer* LP = PlayerController->GetLocalPlayer())
	{
		if(UEnhancedInputLocalPlayerSubsystem* EISubsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			for(const FInputMappingContextAndPriority& Entry : InputMappings)
			{
				if(const UInputMappingContext* IMC = Entry.InputMapping.Get())
				{
					EISubsystem->RemoveMappingContext(IMC);
				}
			}
		}
	}

	ActiveData.ControllersAddedTo.Remove(PlayerController);
}
