// Copyright Epic Games, Inc. All Rights Reserved.

#include "AsyncMixin.h"

DEFINE_LOG_CATEGORY_STATIC(LogAsyncMixin, Log, All);


FAsyncMixin::FLoadingState::FAsyncStep::FAsyncStep(const FSimpleDelegate& InUserCallback)
	: UserCallback(InUserCallback)
{
}

FAsyncMixin::FLoadingState::FAsyncStep::FAsyncStep(const FSimpleDelegate& InUserCallback,
	const TSharedPtr<FStreamableHandle> InStreamableHandle)
		: UserCallback(InUserCallback)
		, StreamingHandle(InStreamableHandle)
{
}

FAsyncMixin::FLoadingState::FAsyncStep::FAsyncStep(const FSimpleDelegate& InUserCallback,
	const TSharedPtr<FAsyncCondition>& InCondition)
		: UserCallback(InUserCallback)
		, Condition(InCondition)
{
}

FAsyncMixin::FLoadingState::FAsyncStep::~FAsyncStep()
{
}

bool FAsyncMixin::FLoadingState::FAsyncStep::ExecuteUserCallback()
{
	const bool Result = UserCallback.ExecuteIfBound();
	UserCallback.Unbind();

	return Result;
}

bool FAsyncMixin::FLoadingState::FAsyncStep::IsComplete() const
{
	//TODO : 
}

void FAsyncMixin::FLoadingState::FAsyncStep::Cancel()
{
}

bool FAsyncMixin::FLoadingState::FAsyncStep::BindCompleteDelegate(const FSimpleDelegate& NewDelegate)
{
}

bool FAsyncMixin::FLoadingState::FAsyncStep::IsCompleteDelegateBound() const
{
}

/** ---------------------------------------------------------------------------------------
				FAsyncCondition
-------------------------------------------------------------------------------------------*/

FAsyncCondition::~FAsyncCondition()
{
	FTSTicker::GetCoreTicker().RemoveTicker(RepeatHandle);
}

bool FAsyncCondition::IsComplete() const
{
	if (UserCondition.IsBound())
	{
		const EAsyncConditionResult Result = UserCondition.Execute();
		return Result == EAsyncConditionResult::Complete;
	}

	// Please see TryToContinue()
	// if Complete,UserCondition will Unbind;
	return true;
}

bool FAsyncCondition::BindCompleteDelegate(const FSimpleDelegate& NewDelegate)
{
	if (IsComplete())
	{
		//Already Complete
		return false;
	}

	CompletionDelegate = NewDelegate;

	if (!RepeatHandle.IsValid())
	{
		RepeatHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateSP(this, &FAsyncCondition::TryToContinue), 0.16);
	}

	return true;
}

bool FAsyncCondition::TryToContinue(float DeltaTime)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_FAsyncCondition_TryToContinue);

	UE_LOG(LogAsyncMixin, Verbose, TEXT("[0x%X] AsyncCondition : TryToContinue"), this)

	if (UserCondition.IsBound())
	{
		const EAsyncConditionResult Result = UserCondition.Execute();

		switch (Result)
		{
		case EAsyncConditionResult::TryAgain:
			return true;
		case EAsyncConditionResult::Complete:
			RepeatHandle.Reset();
			UserCondition.Unbind();

			CompletionDelegate.ExecuteIfBound();
			CompletionDelegate.Unbind();
			break;
		}
	}

	return false;
}
