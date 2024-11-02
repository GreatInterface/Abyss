// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameplayTagContainer.h"
#include "GameFeatureAction_AddWidget.generated.h"

class UCommonActivatableWidget;

USTRUCT()
struct FAbyssHUDLayoutRequest
{
	GENERATED_BODY()

	// The layout widget to spawn
	UPROPERTY(EditAnywhere, Category=UI, meta=(AssetBundles="Client"))
	TSoftClassPtr<UCommonActivatableWidget> LayoutClass;

	//Widget插入的图层(layer)
	UPROPERTY(EditAnywhere, Category=UI, meta=(Categories="UI.Layer"))
	FGameplayTag LayerID;
};

USTRUCT()
struct FAbyssHUDElementEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=UI, meta=(AssetBundles="Client"))
	TSoftClassPtr<UUserWidget> WidgetClass;

	UPROPERTY(EditAnywhere, Category=UI)
	FGameplayTag SlotID;
};

UCLASS(MinimalAPI, meta=(DisplayName="Add Widgets"))
class UGameFeatureAction_AddWidget final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	
public:

	UPROPERTY(EditAnywhere, Category=UI, meta=(TitleProperty="{LayerID} -> {LayoutClass}"))
	TArray<FAbyssHUDLayoutRequest> Layout;

	UPROPERTY(EditAnywhere, Category=UI, meta=(TitleProperty="{SlotID} -> {WidgetClass}"))
	TArray<FAbyssHUDElementEntry> Widgets;

private:
	struct FPerActorData
	{
		TArray<TWeakObjectPtr<UCommonActivatableWidget>> LayoutsAdded;
	};
};
