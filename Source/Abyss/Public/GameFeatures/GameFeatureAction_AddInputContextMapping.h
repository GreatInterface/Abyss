// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_AddInputContextMapping.generated.h"


struct FComponentRequestHandle;
class UInputMappingContext;

USTRUCT()
struct FInputMappingContextAndPriority
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input", meta = (AssetBundles = "Default"))
	TSoftObjectPtr<UInputMappingContext> InputMapping;

	UPROPERTY(EditAnywhere, Category = "Input")
	int32 Priority = 0;

	UPROPERTY(EditAnywhere, Category="Input")
	bool bRegisterWithSettings = true;
};



/**
 *	1. Register -> Register IMC
 *	2. Active -> AddToWorld -> Add IMC
 */
UCLASS(MinimalAPI, meta=(DisplayName = "Add Input Mapping"))
class UGameFeatureAction_AddInputContextMapping final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:

	//~ GameFeatureAction
	virtual void OnGameFeatureRegistering() override;
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void OnGameFeatureUnregistering() override;
	//~ End of GameFeatureAction

	UPROPERTY(EditAnywhere, Category = "Input", meta=(AssetBundles="Client"))
	TArray<FInputMappingContextAndPriority> InputMappings;

private:
	
	FDelegateHandle RegisterInputContextMappingsForGameInstanceHandle;
	
	void RegisterInputMappingContexts();

	void RegisterInputMappingContextsForGameInstance(UGameInstance* GameInstance);

	void RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer);

	void UnregisterInputMappingContexts();

	void UnregisterInputMappingContextsForGameInstance(UGameInstance* GameInstance);

	void UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer);

private:
	//~ WorldActionBase
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
	//~End of WorldActionBase

	struct FPerContextData
	{
		// 存储对扩展处理请求的引用，通过 TSharedPtr 管理扩展处理句柄，
		// 用于在适当时对扩展进行管理（如移除或停用扩展）。
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
		TArray<TWeakObjectPtr<APlayerController>> ControllersAddedTo;
	};

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;
	
	void Reset(FPerContextData& ActiveData);
	void HandleControllerExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
	void AddInputMappingForPlayer(UPlayer* Player, FPerContextData& ActiveData);
	void RemoveInputMappingAndController(APlayerController* PlayerController, FPerContextData& ActiveData);
};
