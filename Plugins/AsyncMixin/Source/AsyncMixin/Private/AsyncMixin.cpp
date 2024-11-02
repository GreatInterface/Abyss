// Copyright Epic Games, Inc. All Rights Reserved.

#include "AsyncMixin.h"

#include "Engine/AssetManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogAsyncMixin, Log, All);

TMap<FAsyncMixin*, TSharedRef<FAsyncMixin::FLoadingState>> FAsyncMixin::Loading;

FAsyncMixin::FAsyncMixin()
{
}

FAsyncMixin::~FAsyncMixin()
{
	check(IsInGameThread())

	Loading.Remove(this);
}

void FAsyncMixin::AsyncLoad(FSoftObjectPath SoftObjectPath, const FSimpleDelegate& Callback)
{
	FindOrAddLoadingState().AsyncLoad(SoftObjectPath, Callback);
}

void FAsyncMixin::AsyncLoad(const TArray<FSoftObjectPath>& SoftObjectPaths, const FSimpleDelegate& Callback)
{
	FindOrAddLoadingState().AsyncLoad(SoftObjectPaths, Callback);
}

void FAsyncMixin::AsyncPreloadPrimaryAssetsAndBundles(const TArray<FPrimaryAssetId>& AssetIds,
	const TArray<FName>& LoadBundles, const FSimpleDelegate& Callback)
{
	FindOrAddLoadingState().AsyncPreloadPrimaryAssetAndBundles(AssetIds, LoadBundles, Callback);
}

void FAsyncMixin::AsyncCondition(TSharedRef<FAsyncCondition> Condition, const FSimpleDelegate& Callback)
{
	FindOrAddLoadingState().AsyncCondition(Condition, Callback);
}

void FAsyncMixin::AsyncEvent(const FSimpleDelegate& Callback)
{
	FindOrAddLoadingState().AsyncEvent(Callback);
}

void FAsyncMixin::StartAsyncLoading()
{
	if (IsLoadingInProgressOrPending())
	{
		FindOrAddLoadingState().Start();
	}
	else
	{
		OnStartedLoading();
		OnFinishedLoading();
	}
}

void FAsyncMixin::CancelAsyncLoading()
{
	if (HasLoadingState())
	{
		FindOrAddLoadingState().CancelAndDestroy();
	}
}

bool FAsyncMixin::IsAsyncLoadingInProgress() const
{
	if (HasLoadingState())
	{
		return GetLoadingStateConst().IsLoadingInProgress();
	}

	return false;
}

const FAsyncMixin::FLoadingState& FAsyncMixin::GetLoadingStateConst() const
{
	check(IsInGameThread());
	return Loading.FindChecked(this).Get();
}

FAsyncMixin::FLoadingState& FAsyncMixin::FindOrAddLoadingState()
{
	check(IsInGameThread());

	if (TSharedRef<FLoadingState>* LoadingState = Loading.Find(this))
	{
		return (*LoadingState).Get();
	}

	return Loading.Add(this, MakeShared<FLoadingState>(*this)).Get();
}

bool FAsyncMixin::HasLoadingState() const
{
	check(IsInGameThread())

	return Loading.Contains(this);
}

bool FAsyncMixin::IsLoadingInProgressOrPending() const
{
	if (HasLoadingState())
	{
		return GetLoadingStateConst().IsLoadingInProgressOrPending();
	}

	return false;
}

/** ---------------------------------------------------------------------------------------
				FLoadingState
-------------------------------------------------------------------------------------------*/



FAsyncMixin::FLoadingState::FLoadingState(FAsyncMixin& InOwner)
	: OwnerRef(InOwner)
{}

FAsyncMixin::FLoadingState::~FLoadingState()
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_FAsyncMixin_FLoadingState_DestroyThisMemoryDelegate);
	UE_LOG(LogAsyncMixin, Verbose, TEXT("[0x%p] Destroy LoadingState (Done)"), this);

	CancelOnly(true);
	CancelDestroyThisMemory(true);
}

void FAsyncMixin::FLoadingState::Start()
{
	UE_LOG(LogAsyncMixin, Verbose, TEXT("[0x%p] Start (Current Progress %d/%d)"), this, CurrentAsyncStep + 1, AsyncSteps.Num());

	CancelStartTimer();
	
	if (!bHasStarted)
	{
		bHasStarted = true;
		//TODO : 
		//OwnerRef.OnStartedLoading();
	}

	TryCompleteAsyncLoading();
}

void FAsyncMixin::FLoadingState::CancelAndDestroy()
{
	CancelOnly(false);
	RequestDestroyThisMemory();
}

void FAsyncMixin::FLoadingState::AsyncLoad(FSoftObjectPath SoftObjectPath, const FSimpleDelegate& DelegateToCall)
{
	UE_LOG(LogAsyncMixin, Verbose, TEXT("[0x%p] AsyncLoad '%s'"), this, *SoftObjectPath.ToString());

	AsyncSteps.Add(MakeUnique<FAsyncStep>(
		DelegateToCall,
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			SoftObjectPath, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority,
			false, false, TEXT("AsyncMixin"))));

	TryScheduleStart();
}

void FAsyncMixin::FLoadingState::AsyncLoad(const TArray<FSoftObjectPath>& SoftObjectPaths,
	const FSimpleDelegate& DelegateToCall)
{
	const FString& Paths = FString::JoinBy(SoftObjectPaths, TEXT(", "), [](const FSoftObjectPath& SoftObjectPath) { return FString::Printf(TEXT("'%s'"), *SoftObjectPath.ToString()); });
	UE_LOG(LogAsyncMixin, Verbose, TEXT("[0x%p] AsyncLoad [%s]"), this, *Paths);

	AsyncSteps.Add(MakeUnique<FAsyncStep>(
		DelegateToCall,
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			SoftObjectPaths, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority,
			false, false, TEXT("AsyncMixin"))));

	TryScheduleStart();
}

void FAsyncMixin::FLoadingState::AsyncPreloadPrimaryAssetAndBundles(const TArray<FPrimaryAssetId>& AssetIds,
	const TArray<FName>& LoadBundles, const FSimpleDelegate& DelegateToCall)
{
	{		
		const FString& Assets = FString::JoinBy(AssetIds, TEXT(", "), [](const FPrimaryAssetId& AssetId) { return AssetId.ToString(); });
		const FString& Bundles = FString::JoinBy(LoadBundles, TEXT(", "), [](const FName& LoadBundle) { return LoadBundle.ToString(); });
		UE_LOG(LogAsyncMixin, Verbose, TEXT("[0x%p]  AsyncPreload Assets [%s], Bundles[%s]"), this, *Assets, *Bundles);
	}

	TSharedPtr<FStreamableHandle> StreamingHandle;

	if (AssetIds.Num() > 0)
	{
		bPreloadedBundles = true;

		const bool bLoadRecursive = true;
		StreamingHandle = UAssetManager::Get().PreloadPrimaryAssets(AssetIds, LoadBundles, bLoadRecursive);
	}

	AsyncSteps.Add(MakeUnique<FAsyncStep>(DelegateToCall, StreamingHandle));

	TryScheduleStart();
}

void FAsyncMixin::FLoadingState::AsyncCondition(TSharedRef<FAsyncCondition> Condition, const FSimpleDelegate& Callback)
{
	UE_LOG(LogAsyncMixin, Verbose, TEXT("[0x%p] AsyncCondition '0x%p'"), this, &Condition.Get());

	AsyncSteps.Add(MakeUnique<FAsyncStep>(Callback, Condition));

	TryScheduleStart();
}

void FAsyncMixin::FLoadingState::AsyncEvent(const FSimpleDelegate& Callback)
{
	UE_LOG(LogAsyncMixin, Verbose, TEXT("[0x%p] AsyncEvent"), this);

	AsyncSteps.Add(MakeUnique<FAsyncStep>(Callback));

	TryScheduleStart();
}

bool FAsyncMixin::FLoadingState::IsLoadingInProgress() const
{
	if (AsyncSteps.Num() > 0)
	{
		if (CurrentAsyncStep < AsyncSteps.Num())
		{
			if (CurrentAsyncStep == (AsyncSteps.Num() - 1))
			{
				return AsyncSteps[CurrentAsyncStep]->IsLoadingInProcess();
			}

			return true;
		}
	}

	return false;
}

bool FAsyncMixin::FLoadingState::IsLoadingInProgressOrPending() const
{
	return StartTimerDelegate.IsValid() || IsLoadingInProgress();
}

bool FAsyncMixin::FLoadingState::IsPendingDestroy() const
{
	return DestroyMemoryDelegate.IsValid();
}

void FAsyncMixin::FLoadingState::CancelOnly(bool bDestroying)
{
	if (!bDestroying)
	{
		UE_LOG(LogAsyncMixin, Verbose, TEXT("[0x%p] Cancel"), this);
	}

	CancelStartTimer();

	for (auto& Step : AsyncSteps)
	{
		Step->Cancel();
	}

	AsyncStepsPendingDestruction = MoveTemp(AsyncSteps);

	bPreloadedBundles = false;
	bHasStarted = false;
	CurrentAsyncStep = 0;
}

void FAsyncMixin::FLoadingState::CancelStartTimer()
{
	if (StartTimerDelegate.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(StartTimerDelegate);
		StartTimerDelegate.Reset();
	}
}

void FAsyncMixin::FLoadingState::TryScheduleStart()
{
	CancelDestroyThisMemory(false);

	if (!StartTimerDelegate.IsValid())
	{
		StartTimerDelegate = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float DeltaTime)
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_FAsyncMixin_FLoadingState_TryScheduleStartDelegate);
			Start();
			return false;
		}));
	}
}

void FAsyncMixin::FLoadingState::TryCompleteAsyncLoading()
{
	if (!bHasStarted)
	{
		return;
	}

	UE_LOG(LogAsyncMixin, Verbose, TEXT("[0x%p] TryCompleteAsyncLoading - (Current Progress %d/%d)"), this, CurrentAsyncStep + 1, AsyncSteps.Num());

	while (CurrentAsyncStep < AsyncSteps.Num())
	{
		FAsyncStep* Step = AsyncSteps[CurrentAsyncStep].Get();
		if (Step->IsLoadingInProcess())
		{
			if (!Step->IsCompleteDelegateBound())
			{
				UE_LOG(LogAsyncMixin, Verbose, TEXT("[0x%p] step %d - still Loading (Listening)"), this, CurrentAsyncStep + 1);
				const bool bBound = Step->BindCompleteDelegate(FSimpleDelegate::CreateSP(this, &FLoadingState::TryCompleteAsyncLoading));
				ensureMsgf(bBound, TEXT("This is not intended to return false.  We're checking if it's loaded above, this should definitely return true."));
			}
			else
			{
				UE_LOG(LogAsyncMixin, Verbose, TEXT("[0x%p] Step %d - Still Loading (Waiting)"), this, CurrentAsyncStep + 1);
			}

			break;
		}

		//!Step->IsLoadingInProcess(),因为上面的分支已经break;

		UE_LOG(LogAsyncMixin, Verbose, TEXT("[0x%p] Step %d - Completed (Calling User)"), this, CurrentAsyncStep + 1);

		CurrentAsyncStep++;

		Step->ExecuteUserCallback();
	}

	if (IsLoadingComplete() && bHasStarted)
	{
		CompleteAsyncLoading();
	}
}

void FAsyncMixin::FLoadingState::CompleteAsyncLoading()
{
	UE_LOG(LogAsyncMixin, Verbose, TEXT("[0x%p] CompleteAsyncLoading"), this);

	if (bHasStarted)
	{
		bHasStarted = false;
		//TODO : OwnerRef.OnFinishedLoading();
	}

	if (IsLoadingComplete())
	{
		if (!bPreloadedBundles && !IsLoadingInProgressOrPending())
		{
			RequestDestroyThisMemory();
		}
	}
}

void FAsyncMixin::FLoadingState::RequestDestroyThisMemory()
{
	if (!IsPendingDestroy())
	{
		UE_LOG(LogAsyncMixin, Verbose, TEXT("[0x%p] Destroy LoadingState (Requested)"), this);

		DestroyMemoryDelegate = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float DeltaTime)
		{
			FAsyncMixin::Loading.Remove(&OwnerRef);
			return false;
		}));
	}
}

void FAsyncMixin::FLoadingState::CancelDestroyThisMemory(bool bDestroying)
{
	if (IsLoadingComplete())
	{
		if (!bDestroying)
		{
			UE_LOG(LogAsyncMixin, Verbose, TEXT("[0x%p] Destroy LoadingState(Cancel)"), this);
		}

		FTSTicker::GetCoreTicker().RemoveTicker(MoveTemp(DestroyMemoryDelegate));
		DestroyMemoryDelegate.Reset();
	}
}


/** ---------------------------------------------------------------------------------------
				FAsyncCondition
-------------------------------------------------------------------------------------------*/

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
	if (StreamingHandle.IsValid())
	{
		return StreamingHandle->HasLoadCompleted();
	}
	if (Condition.IsValid())
	{
		return Condition->IsComplete();
	}

	return true;
}

void FAsyncMixin::FLoadingState::FAsyncStep::Cancel()
{
	if (StreamingHandle.IsValid())
	{
		StreamingHandle->BindCompleteDelegate(FSimpleDelegate());
	}
	else if (Condition.IsValid())
	{
		Condition.Reset();
	}

	bIsCompletionDelegateBound = false;
}

bool FAsyncMixin::FLoadingState::FAsyncStep::BindCompleteDelegate(const FSimpleDelegate& NewDelegate)
{
	if (IsComplete())
	{
		//Too late
		return false;
	}

	if (StreamingHandle.IsValid())
	{
		StreamingHandle->BindCompleteDelegate(NewDelegate);
	}
	else if (Condition.IsValid())
	{
		Condition->BindCompleteDelegate(NewDelegate);
	}

	bIsCompletionDelegateBound = true;

	return true;
}

bool FAsyncMixin::FLoadingState::FAsyncStep::IsCompleteDelegateBound() const
{
	return bIsCompletionDelegateBound;
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

	UE_LOG(LogAsyncMixin, Verbose, TEXT("[0x%p] AsyncCondition : TryToContinue"), this)

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
