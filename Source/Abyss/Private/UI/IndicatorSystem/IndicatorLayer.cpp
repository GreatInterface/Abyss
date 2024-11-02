// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IndicatorSystem/IndicatorLayer.h"

#include "UI/IndicatorSystem/Slate/SActorCanvas.h"

UIndicatorLayer::UIndicatorLayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UIndicatorLayer::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyActorCanvas.Reset();
}

TSharedRef<SWidget> UIndicatorLayer::RebuildWidget()
{
	if (!IsDesignTime())
	{
		ULocalPlayer* LP = GetOwningLocalPlayer();
		if (ensureMsgf(LP, TEXT("Attempting to rebuild a UActorCanvas without a valid LocalPlayer!")))
		{
			MyActorCanvas = SNew(SActorCanvas, FLocalPlayerContext(LP), &ArrowBrush);
			return MyActorCanvas.ToSharedRef();
		}
	}

	return SNew(SBox);
}
