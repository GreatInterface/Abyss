// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayMessageSubsystem.h"
#include "Engine/CancellableAsyncAction.h"
#include "AsyncAction_ListenForGameplayMessage.generated.h"


enum class EGameplayMessageMatch : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAsyncGameplayMessageDelegate, UAsyncAction_ListenForGameplayMessage*, ProxyObject, FGameplayTag, ActualChannel);

UCLASS()
class GAMEPLAYMESSAGERUNTIME_API UAsyncAction_ListenForGameplayMessage : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category=Messaging, meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
	static UAsyncAction_ListenForGameplayMessage* ListenForGameplayMessage(UObject* WorldContextObject, FGameplayTag Channel, UScriptStruct* Payload, EGameplayMessageMatch MatchType = EGameplayMessageMatch::ExactMatch);

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Messaging", meta = (CustomStructureParam = "OutPayload"))
	bool GetPayload(UPARAM(ref) int32& OutPayload);

	DECLARE_FUNCTION(execGetPayload);

	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;
	
public:
	UPROPERTY(BlueprintAssignable)
	FAsyncGameplayMessageDelegate OnMessageReceived;
	
private:
	void HandleMessageReceived(FGameplayTag ChannelTag, const UScriptStruct* StructType, const void* Payload);

private:

	const void* ReceivedMessagePayloadPtr = nullptr;

	TWeakObjectPtr<UWorld> WorldPtr;
	FGameplayTag Channel;
	TWeakObjectPtr<UScriptStruct> MessageStructType = nullptr;
	EGameplayMessageMatch MessageMatchType = EGameplayMessageMatch::ExactMatch;

	FGameplayMessageListenerHandle ListenerHandle;
};
