// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayMessageRuntime/Public/GameplayMessageSubsystem.h"

#include "GameplayMessageRuntime/Public/GameplayMessageType.h"

UGameplayMessageSubsystem& UGameplayMessageSubsystem::Get(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
	check(World);

	UGameplayMessageSubsystem* Router = UGameInstance::GetSubsystem<UGameplayMessageSubsystem>(World->GetGameInstance());
	check(Router)
	
	return *Router;
}

bool UGameplayMessageSubsystem::HasInstance(const UObject* WorldContextObject)
{

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
	UGameplayMessageSubsystem* Router = World ? UGameInstance::GetSubsystem<UGameplayMessageSubsystem>(World->GetGameInstance()) : nullptr;

	return Router != nullptr;
}

void UGameplayMessageSubsystem::Deinitialize()
{
	ListenerMap.Reset();
	
	Super::Deinitialize();
}

void UGameplayMessageSubsystem::BroadcastMessageImpl(FGameplayTag Channel, const UScriptStruct* StructType,
	const void* MessageBytes)
{
	bool bOnInitialTag = true;
	for (FGameplayTag Tag = Channel; Tag.IsValid(); Tag = Tag.RequestDirectParent())
	{
		if (const FChannelListenerList* List = ListenerMap.Find(Tag))
		{
			TArray ListenerArray(List->Listeners);

			for (const FGameplayMessageListenerData& Listener : ListenerArray)
			{
				if (bOnInitialTag || (Listener.MatchType == EGameplayMessageMatch::PartialMatch))
				{
					if (Listener.bHadValidType && !Listener.ListenerStructType.IsValid())
					{
						
					}
				}
			}
		}
	}
}
