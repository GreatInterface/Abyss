#pragma once

#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GameplayTagStack.generated.h"

USTRUCT(BlueprintType)
struct FGameplayTagStack : public FFastArraySerializerItem
{

	GENERATED_BODY()

public:

	FGameplayTagStack() = default;

	FGameplayTagStack(const FGameplayTag& InTag, int32 InStackCount)
		: Tag(InTag)
		, StackCount(InStackCount)
	{}
	

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount = 0;
};


USTRUCT(BlueprintType)
struct FGameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FGameplayTagStackContainer() = default;

	void AddStack(const FGameplayTag& InTag, int32 InStackCount);

	void RemoveStack(const FGameplayTag& InTag, int32 StackCount);

	int32 GetStackCount(const FGameplayTag& InTag) const
	{
		check(InTag.IsValid());

		return TagToCountMap.FindRef(InTag);
	}

	bool ContainsTag(const FGameplayTag& InTag) const
	{
		return TagToCountMap.Contains(InTag);
	}

private:

	UPROPERTY()
	TArray<FGameplayTagStack> StackContainer;

	TMap<FGameplayTag, int32> TagToCountMap;
};
