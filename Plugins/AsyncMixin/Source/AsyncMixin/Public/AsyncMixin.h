#pragma once

#include "Engine/StreamableManager.h"

class FAsyncCondition;

class ASYNCMIXIN_API FAsyncMixin : public FNoncopyable
{

private:

	class FLoadingState : public TSharedFromThis<FLoadingState>
	{

	public:
		FLoadingState(FAsyncMixin& InOwner);
		virtual ~FLoadingState();

		void Start();

		void CancelAndDestroy();


		bool IsLoadingComplete() const {return !IsLoadingInProgress();}
		bool IsLoadingInProgress() const;
		bool IsLoadingInProgressOrPending() const;
		bool IsPendingDestroy() const;
		
	private:
		class FAsyncStep
		{
		public:
			FAsyncStep(const FSimpleDelegate& InUserCallback);
			FAsyncStep(const FSimpleDelegate& InUserCallback, const TSharedPtr<FStreamableHandle> InStreamableHandle);
			FAsyncStep(const FSimpleDelegate& InUserCallback, const TSharedPtr<FAsyncCondition>& InCondition);

			~FAsyncStep();

			bool ExecuteUserCallback();

			bool IsLoadingInProcess() const
			{
				return !IsComplete();
			}

			bool IsComplete() const;
			void Cancel();

			bool BindCompleteDelegate(const FSimpleDelegate& NewDelegate);
			bool IsCompleteDelegateBound() const;
			
		private:
			FSimpleDelegate UserCallback;
			bool bIsCompletionDelegateBound = false;

			TSharedPtr<FStreamableHandle> StreamingHandle;
			TSharedPtr<FAsyncCondition> Condition;
		};

	private:
		void CancelOnly(bool bDestroying);
		void CancelStartTimer();
		void TryScheduleStart();
		void TryCompleteAsyncLoading();
		void CompleteAsyncLoading();
		
		void RequestDestroyThisMemory();
		void CancelDestroyThisMemory(bool bDestroying);
	private:
		FAsyncMixin& OwnerRef;

		bool bPreloadedBundles = false;
		bool bHasStarted = false;

		int32 CurrentAsyncStep = 0;
		TArray<TUniquePtr<FAsyncStep>> AsyncSteps;
		TArray<TUniquePtr<FAsyncStep>> AsyncStepsPendingDestruction;

		FTSTicker::FDelegateHandle StartTimerDelegate;
		FTSTicker::FDelegateHandle DestroyMemoryDelegate;
	};
};

/** ---------------------------------------------------------------------------------------
				Condition
-------------------------------------------------------------------------------------------*/

enum class EAsyncConditionResult : uint8
{
	TryAgain,
	Complete
};

DECLARE_DELEGATE_RetVal(EAsyncConditionResult, FAsyncConditionDelegate);

/**
 *	通过FAsyncCondition ，您可以自定义暂停异步加载的原因，直到满足某些条件。
 */
class FAsyncCondition : public TSharedFromThis<FAsyncCondition>
{
public:
	FAsyncCondition(const FAsyncConditionDelegate&& Condition)
		: UserCondition(MoveTemp(Condition))
	{};

	FAsyncCondition(const TFunction<EAsyncConditionResult()>&& Condition)
		: UserCondition(FAsyncConditionDelegate::CreateLambda([UserFunc = MoveTemp(Condition)]() mutable
		{
			return UserFunc();
		}))
	{};

	virtual ~FAsyncCondition();

protected:

	bool IsComplete() const;
	bool BindCompleteDelegate(const FSimpleDelegate& NewDelegate);

private:

	bool TryToContinue(float DeltaTime);

	friend FAsyncMixin;

	FTSTicker::FDelegateHandle RepeatHandle;
	FAsyncConditionDelegate UserCondition;
	FSimpleDelegate CompletionDelegate;
};