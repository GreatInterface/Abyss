#pragma once

#include "Engine/StreamableManager.h"

class FAsyncCondition;

class ASYNCMIXIN_API FAsyncMixin : public FNoncopyable
{
protected:
	FAsyncMixin();

	virtual void OnStartedLoading() {}

	virtual void OnFinishedLoading() {}

public:
	virtual ~FAsyncMixin();

protected:

	template<typename TObj = UObject>
	void AsyncLoad(TSoftClassPtr<TObj> SoftClass, const FSimpleDelegate& Callback = FSimpleDelegate())
	{
		AsyncLoad(SoftClass.ToSoftObjectPath(), Callback);
	}
	
	template <typename TObj = UObject>
	void AsyncLoad(TSoftClassPtr<TObj> SoftClass, TFunction<void()>&& Callback)
	{
		AsyncLoad(SoftClass.ToSoftObjectPath(), FSimpleDelegate::CreateLambda(MoveTemp(Callback)));		
	}

	template<typename T = UObject>
	void AsyncLoad(TSoftClassPtr<T> SoftClass, TFunction<void(TSubclassOf<T>)>&& Callback)
	{
		AsyncLoad(SoftClass.ToSoftObjectPath(),
			FSimpleDelegate::CreateLambda([SoftClass, UserCallback = MoveTemp(Callback)]() mutable {
				UserCallback(SoftClass.Get());
			})
		);
	}
	
	template <typename TObj = UObject>
	void AsyncLoad(TSoftObjectPtr<TObj> SoftObject, const FSimpleDelegate& Callback = FSimpleDelegate())
	{
		AsyncLoad(SoftObject.ToSoftObjectPath(), Callback);
	}
	
	template <typename TObj = UObject>
	void AsyncLoad(TSoftObjectPtr<TObj> SoftObject, TFunction<void()>&& Callback)
	{
		AsyncLoad(SoftObject.ToSoftObjectPath(), FSimpleDelegate::CreateLambda(MoveTemp(Callback)));
	}

	template <typename TObj = UObject>
	void AsyncLoad(TSoftObjectPtr<TObj> SoftObject, TFunction<void(TObj*)>&& Callback)
	{
		AsyncLoad(SoftObject.ToSoftObjectPath(), FSimpleDelegate::CreateLambda([SoftObject, UserCallback = MoveTemp(Callback)]()
		{
			UserCallback(SoftObject.Get());			
		}));
	}
	
	void AsyncLoad(const TArray<FSoftObjectPath>& SoftObjectPaths, TFunction<void()>&& Callback)
	{
		AsyncLoad(SoftObjectPaths, FSimpleDelegate::CreateLambda(MoveTemp(Callback)));
	}
	
	void AsyncLoad(FSoftObjectPath SoftObjectPath, const FSimpleDelegate& Callback = FSimpleDelegate());
	void AsyncLoad(const TArray<FSoftObjectPath>& SoftObjectPaths, const FSimpleDelegate& Callback = FSimpleDelegate());

	template<typename T = UPrimaryDataAsset>
	void AsyncPreloadPrimaryAssetsAndBundles(const TArray<T*>& Assets, const TArray<FName>& LoadBundles, const FSimpleDelegate& Callback = FSimpleDelegate())
	{
		TArray<FPrimaryAssetId> PrimaryAssetIds;
		for (const T* Item : Assets)
		{
			PrimaryAssetIds.Add(Item);
		}

		AsyncPreloadPrimaryAssetsAndBundles(PrimaryAssetIds, LoadBundles, Callback);
	}

	/** Given an array of primary asset ids, it loads all of the bundles referenced by properties of these assets specified in the LoadBundles array. */
	void AsyncPreloadPrimaryAssetsAndBundles(const TArray<FPrimaryAssetId>& AssetIds, const TArray<FName>& LoadBundles, TFunction<void()>&& Callback)
	{
		AsyncPreloadPrimaryAssetsAndBundles(AssetIds, LoadBundles, FSimpleDelegate::CreateLambda(MoveTemp(Callback)));
	}

	/** Given an array of primary asset ids, it loads all of the bundles referenced by properties of these assets specified in the LoadBundles array. */
	void AsyncPreloadPrimaryAssetsAndBundles(const TArray<FPrimaryAssetId>& AssetIds, const TArray<FName>& LoadBundles, const FSimpleDelegate& Callback = FSimpleDelegate());

	/** Add a future condition that must be true before we move forward. */
	void AsyncCondition(TSharedRef<FAsyncCondition> Condition, const FSimpleDelegate& Callback = FSimpleDelegate());

	/**
	 * Rather than load anything, this callback is just inserted into the callback sequence so that when async loading 
	 * completes this event will be called at the same point in the sequence.  Super useful if you don't want a step to be
	 * tied to a particular asset in case some of the assets are optional.
	 */
	void AsyncEvent(TFunction<void()>&& Callback)
	{
		AsyncEvent(FSimpleDelegate::CreateLambda(MoveTemp(Callback)));
	}

	/**
	 * Rather than load anything, this callback is just inserted into the callback sequence so that when async loading
	 * completes this event will be called at the same point in the sequence.  Super useful if you don't want a step to be
	 * tied to a particular asset in case some of the assets are optional.
	 */
	void AsyncEvent(const FSimpleDelegate& Callback);

	/** Flushes any async loading requests. */
	void StartAsyncLoading();

	/** Cancels any pending async loads. */
	void CancelAsyncLoading();

	/** Is async loading current in progress? */
	bool IsAsyncLoadingInProgress() const;

	
private:

	class FLoadingState : public TSharedFromThis<FLoadingState>
	{

	public:
		FLoadingState(FAsyncMixin& InOwner);
		virtual ~FLoadingState();

		void Start();

		void CancelAndDestroy();

		void AsyncLoad(FSoftObjectPath SoftObjectPath, const FSimpleDelegate& DelegateToCall);
		void AsyncLoad(const TArray<FSoftObjectPath>& SoftObjectPaths, const FSimpleDelegate& DelegateToCall);
		void AsyncPreloadPrimaryAssetAndBundles(const TArray<FPrimaryAssetId>& AssetIds, const TArray<FName>& LoadBundles, const FSimpleDelegate& DelegateToCall);
		void AsyncCondition(TSharedRef<FAsyncCondition> Condition, const FSimpleDelegate& Callback);
		void AsyncEvent(const FSimpleDelegate& Callback);
		
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

	const FLoadingState& GetLoadingStateConst() const;
	FLoadingState& FindOrAddLoadingState();

	bool HasLoadingState() const;

	bool IsLoadingInProgressOrPending() const;
	
private:
	static TMap<FAsyncMixin*, TSharedRef<FLoadingState>> Loading;
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
	FAsyncCondition(const FAsyncConditionDelegate& Condition)
		: UserCondition(Condition)
	{};

	FAsyncCondition(TFunction<EAsyncConditionResult()>&& Condition)
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