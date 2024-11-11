// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatures/GameFeatureAction_SplitscreenConfig.h"

#define LOCTEXT_NAMESPACE "AbyssGameFeatures"

void UGameFeatureAction_SplitscreenConfig::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	auto ViewportIt = LocalDisableVotes.CreateIterator();
	for (; ViewportIt; ++ViewportIt)
	{
		UGameViewportClient* VC = Cast<UGameViewportClient>(ViewportIt->ResolveObjectPtr());
		const FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(VC);
		if (VC && WorldContext)
		{
			if (!Context.ShouldApplyToWorldContext(*WorldContext))
			{
				continue;
			}
		}

		int32& VoteCount = GlobalDisableVotes[*ViewportIt];
		if (VoteCount <= 1)
		{
			GlobalDisableVotes.Remove(*ViewportIt);

			if (VC && WorldContext)
			{
				VC->SetForceDisableSplitscreen(false);
			}
		}
		else
		{
			--VoteCount;
		}

		ViewportIt.RemoveCurrent();
	}
}

void UGameFeatureAction_SplitscreenConfig::AddToWorld(const FWorldContext& WorldContext,
	const FGameFeatureStateChangeContext& ChangeContext)
{
	if (bDisableSplitscreen)
	{
		if (UGameInstance* GameInstance = WorldContext.OwningGameInstance)
		{
			if (UGameViewportClient* VC = GameInstance->GetGameViewportClient())
			{
				FObjectKey ViewportKey(VC);

				LocalDisableVotes.Add(ViewportKey);

				int32& VoteCount = GlobalDisableVotes.FindOrAdd(ViewportKey);
				VoteCount++;
				if (VoteCount == 1)
				{
					VC->SetForceDisableSplitscreen(true);
				}
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE