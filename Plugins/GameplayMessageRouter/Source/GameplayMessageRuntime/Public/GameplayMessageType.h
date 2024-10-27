#pragma once

#include "GameplayMessageType.generated.h"

struct FGameplayTag;

UENUM(BlueprintType)
enum class EGameplayMessageMatch : uint8
{
	ExactMatch,

	PartialMatch
};


USTRUCT()
struct FGameplayMessageListenerData
{
	GENERATED_BODY()

	TFunction<void(const FGameplayTag&, const UScriptStruct*, const void*)> ReceivedCallback;

	int32 HandleID;
	EGameplayMessageMatch MatchType;

	TWeakObjectPtr<const UScriptStruct> ListenerStructType = nullptr;
	bool bHadValidType = false;
};

USTRUCT()
struct GAMEPLAYMESSAGERUNTIME_API FGameplayMessageListenerHandle
{
	GENERATED_BODY()
	
};
