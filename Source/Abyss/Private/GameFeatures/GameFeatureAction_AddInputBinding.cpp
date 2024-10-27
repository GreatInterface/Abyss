// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatures/GameFeatureAction_AddInputBinding.h"
#include "EnhancedInputSubsystems.h"
#include "Character/Component/AbyssHeroComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Input/AbyssInputConfig.h"

void UGameFeatureAction_AddInputBinding::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);
	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) ||
		!ensure(ActiveData.PawnAddedTo.IsEmpty()))
	{
		Reset(ActiveData);
	}
	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddInputBinding::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	FPerContextData* ActiveData = ContextData.Find(Context);
	if(ensure(ActiveData))
	{
		Reset(*ActiveData);
	}
}

void UGameFeatureAction_AddInputBinding::AddToWorld(const FWorldContext& WorldContext,
                                                    const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if (GameInstance && World && World->IsGameWorld())
	{
		if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			auto AddAbilityDelegate =
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandlePawnExtension, ChangeContext);

			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle =
				ComponentManager->AddExtensionHandler(APawn::StaticClass(), AddAbilityDelegate);

			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
	}
}

void UGameFeatureAction_AddInputBinding::Reset(FPerContextData& ActiveData)
{
	ActiveData.ExtensionRequestHandles.Empty();

	while (!ActiveData.PawnAddedTo.IsEmpty())
	{
		TWeakObjectPtr<APawn> PawnPtr = ActiveData.PawnAddedTo.Top();
		if(PawnPtr.IsValid())
		{
			RemoveInputConfig(PawnPtr.Get(), ActiveData);
		}
		else
		{
			ActiveData.PawnAddedTo.Pop();
		}
	}
}

void UGameFeatureAction_AddInputBinding::HandlePawnExtension(AActor* Actor, FName EventName,
	FGameFeatureStateChangeContext ChangeContext)
{
	APawn* AsPawn = CastChecked<APawn>(Actor);
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		RemoveInputConfig(AsPawn, ActiveData);
	}
	else if((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UAbyssHeroComponent::NAME_BindInputsNow))
	{
		AddInputConfigForPlayer(AsPawn, ActiveData);
	}
	else
	{
		UE_LOG(LogGameFeatures, Warning, TEXT("Can't Add Or Remove Input Mapping"));
	}
}

void UGameFeatureAction_AddInputBinding::AddInputConfigForPlayer(APawn* Pawn, FPerContextData& ActiveData)
{
	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	
	if (ULocalPlayer* LP = PC ? PC->GetLocalPlayer() : nullptr; LP)
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			UAbyssHeroComponent* HeroComp = UAbyssHeroComponent::FindHeroComponent(Pawn);
			if (HeroComp && HeroComp->IsReadyToBindInputs())
			{
				for (const TSoftObjectPtr<const UAbyssInputConfig>& Entry : InputConfigs)
				{
					if (const UAbyssInputConfig* Config = Entry.Get())
					{
						TArray<uint32> BindHandles;
						HeroComp->AddAdditionalInputConfig(Config, OUT BindHandles);
						ConfigBindHandlesMap.Add(Config, BindHandles);
					}
				}
			}
			ActiveData.PawnAddedTo.AddUnique(Pawn);
		}
		else
		{
			UE_LOG(LogGameFeatures, Error, TEXT("Failed to find UEnhancedInputLocalPlayerSubsystem"));
		}
	}
}

void UGameFeatureAction_AddInputBinding::RemoveInputConfig(APawn* Pawn, FPerContextData& ActiveData)
{
	UAbyssHeroComponent* HeroComp = UAbyssHeroComponent::FindHeroComponent(Pawn);
	if (HeroComp && HeroComp->IsReadyToBindInputs())
	{
		for(const auto& Entry : InputConfigs)
		{
			if(const UAbyssInputConfig* Config = Entry.Get())
			{
				if (TArray<uint32>* BindHandles = ConfigBindHandlesMap.Find(Config))
				{
					HeroComp->RemoveAdditionalInputConfigByHandles(*BindHandles);
					ConfigBindHandlesMap.Remove(Config);
				}
			}
		}
	}
	ActiveData.PawnAddedTo.Remove(Pawn);
}
