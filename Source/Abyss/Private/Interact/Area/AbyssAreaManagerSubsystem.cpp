// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/Area/AbyssAreaManagerSubsystem.h"
#include "EngineUtils.h"
#include "Abyss/AbyssLogChannels.h"
#include "Interact/AbyssInteractable_AreaGate.h"


void UAbyssAreaManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &ThisClass::HandlePreLoadMap);
}

void UAbyssAreaManagerSubsystem::Deinitialize()
{
	FCoreUObjectDelegates::PreLoadMapWithContext.RemoveAll(this);

	HandleUnloadMap();
}

void UAbyssAreaManagerSubsystem::BeginEntryArea(FAbyssAreaHandle AreaHandle)
{
	//根据Handle通知对象池里面的Interactable进行激活
	UE_LOG(LogAbyss, Warning, TEXT("BeginEntryArea"));
}

void UAbyssAreaManagerSubsystem::BeginExitArea(FAbyssAreaHandle AreaHandle)
{
	//关闭区域内的互动
}

void UAbyssAreaManagerSubsystem::HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName)
{
	UWorld* World = WorldContext.World();
	auto AreaIt = TActorIterator<AAbyssInteractable_AreaGate>(World);
	for (; AreaIt; ++AreaIt)
	{
		AreaGatesMap.FindOrAdd(AreaIt->GetHandle(), *AreaIt);
	}

	if (AreaGatesMap.Num() > 0)
	{
		OnEnteredArea.AddUObject(this, &ThisClass::BeginEntryArea);
		OnExitedArea.AddUObject(this, &ThisClass::BeginExitArea);
	}

	UE_LOG(LogAbyss, Log, TEXT("UAbyssAreaManagerSubsystem::OnLevelInit"));
}

void UAbyssAreaManagerSubsystem::HandleUnloadMap()
{
	AreaGatesMap.Reset();
	
	OnEnteredArea.RemoveAll(this);
	OnExitedArea.RemoveAll(this);

	UE_LOG(LogAbyss, Log, TEXT("UAbyssAreaManagerSubsystem::OnLevelClosed"));
}
