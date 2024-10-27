#pragma once
#include "GameplayTagContainer.h"

#include "GameplayMessageType.generated.h"

class UGameplayMessageSubsystem;
struct FGameplayTag;

UENUM(BlueprintType)
enum class EGameplayMessageMatch : uint8
{
	ExactMatch,

	PartialMatch
};

template<typename FMessageStructType>
struct FGameplayMessageListenerParams
{

	template<typename TOwner = UObject>
	void SetMessageReceivedCallback(TOwner* Obj, void(TOwner::* Function)(FGameplayTag, const FMessageStructType&))
	{
		TWeakObjectPtr<TOwner> WeakObject(Obj);
		OnMessageReceivedCallback = [WeakObject, Function](FGameplayTag Channel, const FMessageStructType& Payload)
		{
			if (TOwner* StrongObj = WeakObject.Get())
			{
				(StrongObj->*Function)(Channel, Payload);
			}
		};
	}
	
public:
	EGameplayMessageMatch MatchType = EGameplayMessageMatch::ExactMatch;

	TFunction<void(FGameplayTag, const FMessageStructType&)> OnMessageReceivedCallback;
};


//---------------------------------------------------------------------------------------------------

USTRUCT()
struct FGameplayMessageListenerData
{
	GENERATED_BODY()

	TFunction<void(FGameplayTag, const UScriptStruct*, const void*)> ReceivedCallback;

	int32 HandleID;
	EGameplayMessageMatch MatchType;

	TWeakObjectPtr<const UScriptStruct> ListenerStructType = nullptr;
	bool bHadValidType = false;
};

USTRUCT(BlueprintType)
struct GAMEPLAYMESSAGERUNTIME_API FGameplayMessageListenerHandle
{
	GENERATED_BODY()

	FGameplayMessageListenerHandle(){}

	void Unregister();

	bool IsValid() const { return ID != 0; }

private:

	friend class UGameplayMessageSubsystem;
	
	FGameplayMessageListenerHandle(UGameplayMessageSubsystem* InSubsystem, FGameplayTag InChannel, int32 InID)
		: Subsystem(InSubsystem)
		, Channel(InChannel)
		, ID(InID)
	{}
	
private:
	UPROPERTY(Transient)
	TWeakObjectPtr<UGameplayMessageSubsystem> Subsystem;
	
	UPROPERTY(Transient)
	FGameplayTag Channel;
	
	UPROPERTY(Transient)
	int32 ID = 0;

	FDelegateHandle StateClearedHandle;
};
