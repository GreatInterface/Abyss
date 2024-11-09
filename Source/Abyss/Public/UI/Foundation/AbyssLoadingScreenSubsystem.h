// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbyssLoadingScreenSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadingScreenWidgetChangedDelegate, TSubclassOf<UUserWidget>, NewWidgetClass);

UCLASS()
class ABYSS_API UAbyssLoadingScreenSubsystem final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UAbyssLoadingScreenSubsystem();

	UFUNCTION(BlueprintCallable)
	void SetLoadingScreenContextWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	UFUNCTION(BlueprintPure)
	TSubclassOf<UUserWidget> GetLoadingScreenContextWidget();

private:
	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess=true))
	FLoadingScreenWidgetChangedDelegate OnLoadingScreenWidgetChanged;

	UPROPERTY()
	TSubclassOf<UUserWidget> LoadingScreenWidgetClass;
};
