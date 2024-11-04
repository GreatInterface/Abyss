// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatures/GameFeatureAction_AddWidget.h"

#include "CommonActivatableWidget.h"
#include "CommonUIExtensions.h"
#include "Components/GameFrameworkComponentManager.h"
#include "UI/Extension/UIExtensionSubsystem.h"
#include "UI/Framework/AbyssHUD.h"

void UGameFeatureAction_AddWidget::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
}

void UGameFeatureAction_AddWidget::AddToWorld(const FWorldContext& WorldContext,
	const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GI = WorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if (GI && World && World->IsGameWorld())
	{
		if (UGameFrameworkComponentManager* Manager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GI))
		{
			TSoftClassPtr<AActor> HUDClass = AAbyssHUD::StaticClass();

			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = Manager->AddExtensionHandler(
				HUDClass,
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(
					this, &ThisClass::HandleActorExtension, ChangeContext));

			ActiveData.ComponentRequests.Add(ExtensionRequestHandle);
		}
	}
}

void UGameFeatureAction_AddWidget::Reset(FPerContextData& ActiveData)
{
	ActiveData.ComponentRequests.Empty();

	for (TPair<FObjectKey, FPerActorData>& Pair : ActiveData.ActorData)
	{
		for (FUIExtensionHandle Handle : Pair.Value.ExtensionHandles)
		{
			Handle.Unregister();
		}
	}

	ActiveData.ActorData.Empty();
}

void UGameFeatureAction_AddWidget::HandleActorExtension(AActor* Actor, FName EventName,
                                                        FGameFeatureStateChangeContext ChangeContext)
{
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);
	
	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) ||
		(EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		RemoveWidget(Actor, ActiveData);
	}

	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) ||
		(EventName == UGameFrameworkComponentManager::NAME_GameActorReady))
	{
		AddWidget(Actor, ActiveData);
	}
}

void UGameFeatureAction_AddWidget::AddWidget(AActor* Actor, FPerContextData& ActiveData)
{
	AAbyssHUD* HUD = CastChecked<AAbyssHUD>(Actor);

	if (!HUD->GetOwningPlayerController())
		return;

	if (ULocalPlayer* LP = Cast<ULocalPlayer>(HUD->GetOwningPlayerController()->Player))
	{
		FPerActorData& ActorData = ActiveData.ActorData.FindOrAdd(HUD);

		for (const FAbyssHUDLayoutRequest& Request : Layout)
		{
			TSubclassOf<UCommonActivatableWidget> ConcreteWidgetClass = Request.LayoutClass.Get();
			if (ConcreteWidgetClass)
			{
				return;
			}

			ActorData.LayoutsAdded.Add(UCommonUIExtensions::PushContentToLayer_ForPlayer(LP, Request.LayerID, ConcreteWidgetClass));
		}

		UUIExtensionSubsystem* Subsystem = HUD->GetWorld()->GetSubsystem<UUIExtensionSubsystem>();
		for (const FAbyssHUDElementEntry& Entry : Widgets)
		{
			ActorData.ExtensionHandles.Add(Subsystem->RegisterExtensionAsWidgetForContext(LP, Entry.SlotID, Entry.WidgetClass.Get(), -1));
		}
	}
}

void UGameFeatureAction_AddWidget::RemoveWidget(AActor* Actor, FPerContextData& ActiveData)
{
	AAbyssHUD* HUD = CastChecked<AAbyssHUD>(Actor);

	FPerActorData* ActorData = ActiveData.ActorData.Find(HUD); 
	if (ActorData)
	{
		for (TWeakObjectPtr<UCommonActivatableWidget>& AddedLayout : ActorData->LayoutsAdded)
		{
			if (AddedLayout.IsValid())
			{
				AddedLayout->DeactivateWidget();
			}
		}

		for (FUIExtensionHandle& Handle : ActorData->ExtensionHandles)
		{
			Handle.Unregister();
		}
	}
}
