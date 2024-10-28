// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayMessageType.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayMessageSubsystem.generated.h"

enum class EGameplayMessageMatch : uint8;
struct FGameplayMessageListenerHandle;
struct FGameplayTag;
struct FGameplayMessageListenerData;

GAMEPLAYMESSAGERUNTIME_API DECLARE_LOG_CATEGORY_EXTERN(LogGameplayMessageSubsystem, Log, All);

/**
 * 用于管理和广播消息的GameInstanceSubsystem子系统类
 * 支持在多个频道（通过FGameplayTag Channel）上注册监听器，以响应不同类型的消息
 */
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

	/**
	 * 广播消息
	 * 
	 * @tparam FMessageStructType 消息结构体类型
	 * @param Channel 频道标签
	 * @param Message 需要广播的结构体
	 */
	template<typename FMessageStructType>
	void BroadcastMessage(FGameplayTag Channel, const FMessageStructType& Message)
	{
		const UScriptStruct* StructType = TBaseStructure<FMessageStructType>::Get();
		BroadcastMessageImpl(Channel, StructType, &Message);
	}
	
	template<typename FMessageStructType>
	FGameplayMessageListenerHandle RegisterListener(FGameplayTag Channel,
		FGameplayMessageListenerParams<FMessageStructType>& Params)
	{
		FGameplayMessageListenerHandle Handle;

		if (Params.OnMessageReceivedCallback)
		{
			auto ThunkCallback = [InnerCallback = Params.OnMessageReceivedCallback](
				FGameplayTag ActualTag, const UScriptStruct* SenderStructType, const void* SenderPayload)
			{
				InnerCallback(ActualTag, *reinterpret_cast<const FMessageStructType*>(SenderPayload));
			};

			const UScriptStruct* StructType = TBaseStructure<FMessageStructType>::Get();
			Handle = RegisterListenerImpl(Channel, ThunkCallback, StructType, Params.MatchType);
		}

		return Handle;
	}

	template<typename FMessageStructType, typename TOwner = UObject>
	FGameplayMessageListenerHandle RegisterListener(FGameplayTag Channel, TOwner* Obj,
		void(TOwner::* Function)(FGameplayTag, const FMessageStructType&))
	{
		TWeakObjectPtr<TOwner> WeakObject(Obj);
		return RegisterListener<FMessageStructType>(Channel,
			[WeakObject, Function](FGameplayTag Channel, const FMessageStructType& Payload)
			{
				if (TOwner* StringObj = WeakObject.Get())
				{
					(StringObj->*Function)(Channel, Payload);
				}
			});
	}

	template<typename FMessageStructType>
	FGameplayMessageListenerHandle RegisterListener(
		FGameplayTag Channel,
		TFunction<void(FGameplayTag, const FMessageStructType&)>&& Callback,
		EGameplayMessageMatch MatchType = EGameplayMessageMatch::ExactMatch)
	{
		auto ThunkCallback = [InnerCallback = MoveTemp(Callback)](FGameplayTag ActualTag,
			const UScriptStruct* SenderStructType, const void* SenderPayload)
		{
			InnerCallback(ActualTag, *reinterpret_cast<const FMessageStructType*>(SenderStructType));	
		};

		const UScriptStruct* StructType = TBaseStructure<FMessageStructType>::Get();
		return RegisterListenerImpl(Channel, ThunkCallback, StructType, MatchType);
	}
	
	void UnregisterListener(FGameplayMessageListenerHandle Handle);

protected:

	UFUNCTION(BlueprintCallable, CustomThunk, Category=Messaging, meta=(CustomStructureParam="Message", AllowAbstrast="false", DisplayName="Broadcast Message"))
	void K2_BroadcastMessage(FGameplayTag Channel, const int32& Message);

	DECLARE_FUNCTION(execK2_BroadcastMessage);

private:

	void BroadcastMessageImpl(FGameplayTag Channel, const UScriptStruct* StructType, const void* MessageBytes);

	FGameplayMessageListenerHandle RegisterListenerImpl(
		FGameplayTag Channel,
		TFunction<void(FGameplayTag, const UScriptStruct*, const void*)>&& Callback,
		const UScriptStruct* StructType,
		EGameplayMessageMatch MatchType);

	void UnregisterListenerImpl(FGameplayTag Channel, int32 HandleID);

private:
	struct FChannelListenerList
	{
		TArray<FGameplayMessageListenerData> ListenerDatas;
		int32 HandleID = 0;
	};

	TMap<FGameplayTag, FChannelListenerList> TagToListenerMap;
};
