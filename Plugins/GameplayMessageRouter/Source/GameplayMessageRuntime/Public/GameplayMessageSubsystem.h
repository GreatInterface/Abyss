// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayMessageSubsystem.generated.h"

struct FGameplayTag;
struct FGameplayMessageListenerData;

UCLASS()
class GAMEPLAYMESSAGERUNTIME_API UGameplayMessageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	static UGameplayMessageSubsystem& Get(const UObject* WorldContextObject);

	/**
	 * @return true if 在当前World中存在一个已经激活的GameplayMessageSubsystem
	 */
	static bool HasInstance(const UObject* WorldContextObject);

	virtual void Deinitialize() override;

	template<typename FMessageStructType>
	void BroadcastMessage(FGameplayTag Channel, const FMessageStructType& Message)
	{
		const UScriptStruct* StructType = TBaseStructure<FMessageStructType>::Get();
		//TODO :
		unimplemented();
	}

private:

	void BroadcastMessageImpl(FGameplayTag Channel, const UScriptStruct* StructType, const void* MessageBytes);

private:
	struct FChannelListenerList
	{
		TArray<FGameplayMessageListenerData> Listeners;
		int32 HandleID = 0;
	};

	TMap<FGameplayTag, FChannelListenerList> ListenerMap;
};
