#include "System/GameplayTag/GameplayTagStack.h"

void FGameplayTagStackContainer::AddStack(const FGameplayTag& InTag, int32 InStackCount)
{
	if (!InTag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("An Invaild Tag was passed to AddStack"), ELogVerbosity::Warning);
		return;
	}

	if (InStackCount > 0)
	{
		for (FGameplayTagStack& Stack : StackContainer)
		{
			if (Stack.Tag == InTag)
			{
				Stack.StackCount += InStackCount;
				TagToCountMap[ InTag ] = Stack.StackCount;
				MarkItemDirty(Stack);
				return;
			}
		}

		/** 如果容器里没有这个Tag的记录，将这个Tag加到容器里 */
		FGameplayTagStack& NewStack = StackContainer.Emplace_GetRef(InTag, InStackCount);
		MarkItemDirty(NewStack);
		TagToCountMap.Add(InTag, InStackCount);
	}
}

void FGameplayTagStackContainer::RemoveStack(const FGameplayTag& InTag, int32 StackCount)
{
	if (!InTag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag was passed to RemoveStack"), ELogVerbosity::Warning);
		return;
	}

	for (auto It = StackContainer.CreateIterator(); It; ++It)
	{
		FGameplayTagStack& Stack = *It;
		if (Stack.Tag == InTag)
		{
			if (Stack.StackCount <= StackCount)
			{
				It.RemoveCurrent();
				TagToCountMap.Remove(InTag);
				MarkArrayDirty();
			}
			else
			{
				Stack.StackCount -= StackCount;
				TagToCountMap[ InTag ] = Stack.StackCount;
				MarkItemDirty(Stack);
			}
			return;
		}
	}
}
