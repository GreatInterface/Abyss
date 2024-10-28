// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayMessageRuntime/Public/GameplayMessageSubsystem.h"
#include "GameplayMessageRuntime/Public/GameplayMessageType.h"


DEFINE_LOG_CATEGORY(LogGameplayMessageSubsystem);

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
	TagToListenerMap.Reset();
	
	Super::Deinitialize();
}

void UGameplayMessageSubsystem::UnregisterListener(FGameplayMessageListenerHandle Handle)
{
	if (Handle.IsValid())
	{
		check(Handle.Subsystem == this);

		UnregisterListenerImpl(Handle.Channel, Handle.ID);
	}
	else
	{
		UE_LOG(LogGameplayMessageSubsystem, Warning, TEXT("Trying to Unregister an invalid Handle"));
	}
}

void UGameplayMessageSubsystem::K2_BroadcastMessage(FGameplayTag Channel, const int32& Message)
{
	checkNoEntry();
}

void UGameplayMessageSubsystem::BroadcastMessageImpl(FGameplayTag Channel, const UScriptStruct* StructType,
                                                     const void* MessageBytes)
{
	bool bOnInitialTag = true;
	for (FGameplayTag Tag = Channel; Tag.IsValid(); Tag = Tag.RequestDirectParent())
	{
		if (const FChannelListenerList* List = TagToListenerMap.Find(Tag))
		{
			TArray ListenerArray(List->ListenerDatas);

			for (const FGameplayMessageListenerData& Data : ListenerArray)
			{
				if (bOnInitialTag || (Data.MatchType == EGameplayMessageMatch::PartialMatch))
				{
					if (Data.bHadValidType && !Data.ListenerStructType.IsValid())
					{
						UE_LOG(LogGameplayMessageSubsystem, Warning, TEXT("Listener struct type has gone invalid on Channel %s. Removing listener from list"), *Channel.ToString());
						UnregisterListenerImpl(Channel, Data.HandleID);
						continue;
					}

					if (!Data.bHadValidType || StructType->IsChildOf(Data.ListenerStructType.Get()))
					{
						Data.ReceivedCallback(Channel, StructType, MessageBytes);
					}
					else
					{
						UE_LOG(LogGameplayMessageSubsystem, Error, TEXT("频道 %s 上的结构类型不匹配（广播类型 %s，位于 %s 的监听器期望类型 %s）"),
							*Channel.ToString(),
							*StructType->GetPathName(),
							*Tag.ToString(),
							*Data.ListenerStructType->GetPathName());
						
					}
				}
			}
		}

		bOnInitialTag = false;
	}
}

FGameplayMessageListenerHandle UGameplayMessageSubsystem::RegisterListenerImpl(FGameplayTag Channel,
	TFunction<void(FGameplayTag, const UScriptStruct*, const void*)>&& Callback, const UScriptStruct* StructType,
	EGameplayMessageMatch MatchType)
{
	FChannelListenerList& List = TagToListenerMap.FindOrAdd(Channel);

	FGameplayMessageListenerData& Entry = List.ListenerDatas.AddDefaulted_GetRef();
	Entry.ReceivedCallback = MoveTemp(Callback);
	Entry.ListenerStructType = StructType;
	Entry.bHadValidType = (StructType != nullptr);
	Entry.HandleID = ++List.HandleID;
	Entry.MatchType = MatchType;

	return FGameplayMessageListenerHandle(this, Channel, Entry.HandleID);
}

void UGameplayMessageSubsystem::UnregisterListenerImpl(FGameplayTag Channel, int32 HandleID)
{
	if (FChannelListenerList* List = TagToListenerMap.Find(Channel))
	{
		int32 MatchIndex = List->ListenerDatas.IndexOfByPredicate([ID = HandleID](const FGameplayMessageListenerData& Data)
		{
			return Data.HandleID == ID;
		});

		if (MatchIndex != INDEX_NONE)
		{
			List->ListenerDatas.RemoveAtSwap(MatchIndex);
		}

		if (List->ListenerDatas.IsEmpty())
		{
			TagToListenerMap.Remove(Channel);
		}
	}
}
