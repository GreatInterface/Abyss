// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <LoadingProcessInterface.h>
#include "CoreMinimal.h"
#include "CommonUserSubsystem.h"
#include "ControlFlowNode.h"
#include "Components/GameStateComponent.h"
#include "GameModes/Experience/AbyssExperienceDefinition.h"
#include "AbyssFrontendStateComponent.generated.h"


class UCommonActivatableWidget;

UCLASS(Abstract)
class ABYSS_API UAbyssFrontendStateComponent : public UGameStateComponent, public ILoadingProcessInterface
{
	GENERATED_BODY()

public:
	UAbyssFrontendStateComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	//~ILoadingProcessInterface interface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	//~End of ILoadingProcessInterface interface

private:
	void OnExperienceLoad(const UAbyssExperienceDefinition* InExperience);

	void FlowStep_WaitForUserInitialization(FControlFlowNodeRef SubFlow);
	void FlowStep_TryShowPressStartScreen(FControlFlowNodeRef SubFlow);
	void FlowStep_TryJoinRequestedSession(FControlFlowNodeRef SubFlow);
	void FlowStep_TryShowMainScreen(FControlFlowNodeRef SubFlow);

	UFUNCTION()
	void OnUserInitialized(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext);
	
	
	TSharedPtr<FControlFlow> FrontEndFlow;

	FControlFlowNodePtr InProgressPressStartScreen;

	bool bShouldShowLoadingScreen = true;
	
	UPROPERTY(EditAnywhere, Category=UI)
	TSoftClassPtr<UCommonActivatableWidget> PressStartScreenClass;

	UPROPERTY(EditAnywhere, Category=UI)
	TSoftClassPtr<UCommonActivatableWidget> MainScreenClass;
};
