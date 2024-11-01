#include "UI/IndicatorSystem/Slate/SActorCanvas.h"
#include "UI/IndicatorSystem/AbyssIndicatorManagerComponent.h"
#include "UI/IndicatorSystem/IndicatorWidgetInterface.h"

void SActorCanvas::Construct(const FArguments& InArgs, const FLocalPlayerContext& InCtx,
                             const FSlateBrush* InActorCanvasArrowBrush)
{
	LocalPlayerContext = InCtx;
	ActorCanvasArrowBrush = InActorCanvasArrowBrush;

	IndicatorPool.SetWorld(LocalPlayerContext.GetWorld());

	SetCanTick(true);
	SetVisibility(EVisibility::SelfHitTestInvisible);

	for (int32 Idx = 0; Idx < 10; ++Idx)
	{
		TSharedRef<SActorCanvasArrowWidget> ArrowWidget = SNew(SActorCanvasArrowWidget, ActorCanvasArrowBrush);
		ArrowWidget->SetVisibility(EVisibility::Collapsed);

		ArrowChildren.AddSlot(MoveTemp(
			FArrowSlot::FSlotArguments(MakeUnique<FArrowSlot>())
			[
				ArrowWidget	
			]
		));
	}

	UpdateActiveTimer();
}

EActiveTimerReturnType SActorCanvas::UpdateCanvas(double InCurrentTime, float InDeltaTime)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_SActorCanvas_UpdateCanvas);

	if (!OptionalPaintGeometry.IsSet())
	{
		return EActiveTimerReturnType::Continue;
	}

	ULocalPlayer* LP = LocalPlayerContext.GetLocalPlayer();
	UAbyssIndicatorManagerComponent* IndicatorManagerComp = IndicatorCompPtr.Get();
	if (IndicatorManagerComp == nullptr)
	{
		IndicatorManagerComp = UAbyssIndicatorManagerComponent::GetComponent(LocalPlayerContext.GetPlayerController());
		if (IndicatorManagerComp)
		{
			IndicatorPool.SetWorld(LocalPlayerContext.GetWorld());

			IndicatorCompPtr = IndicatorManagerComp;

			IndicatorManagerComp->OnIndicatorAddedEvent.AddSP(this, &SActorCanvas::OnIndicatorAdded);
			IndicatorManagerComp->OnIndicatorRemovedEvent.AddSP(this, &SActorCanvas::OnIndicatorRemoved);
			for (UIndicatorDescriptor* Indicator : IndicatorManagerComp->GetIndicators())
			{
				OnIndicatorAdded(Indicator);
			}
		}
		else
		{
			return EActiveTimerReturnType::Continue;
		}
	}

	if (LP)
	{
		const FGeometry PaintGeometry = OptionalPaintGeometry.GetValue();

		FSceneViewProjectionData ProjectionData;
		if (LP->GetProjectionData(LP->ViewportClient->Viewport, ProjectionData))
		{
			SetShowAnyIndicators(true);

			bool IndicatorsChanged = false;

			for (int32 ChildIdx = 0; ChildIdx < CanvasChildren.Num(); ++ChildIdx)
			{
				SActorCanvas::FSlot& CurrentChild = CanvasChildren[ChildIdx];
				UIndicatorDescriptor* Indicator = CurrentChild.Indicator;

				//Slot content is invalid and we have permission(权限) to remove it
				if (Indicator->CanAutomaticallyRemove())
				{
					IndicatorsChanged = true;

					RemoveIndicatorForEntry(Indicator);

					--ChildIdx;
					continue;
				}

				CurrentChild.SetIsIndicatorVisible(Indicator->GetVisible());

				if (!CurrentChild.GetIsIndicatorVisible())
				{
					IndicatorsChanged |= CurrentChild.IsDirty();
					CurrentChild.ClearDirtyFlag();
					continue;
				}

				if (CurrentChild.WasIndicatorClampedStatusChanged())
				{
					CurrentChild.ClearIndicatorClampedStatusChangedFlag();
					IndicatorsChanged = true;
				}

				FVector ScreenPositionWithDepth;

				FIndicatorProjection Projection;
				const bool bSuccess = Projection.Project(*Indicator, ProjectionData, PaintGeometry.Size, OUT ScreenPositionWithDepth);

				if (!bSuccess)
				{
					CurrentChild.SetHasValidScreenPosition(false);
					CurrentChild.SetInFrontOfCamera(false);

					IndicatorsChanged |= CurrentChild.IsDirty();
					CurrentChild.ClearDirtyFlag();
					continue;
				}

				CurrentChild.SetInFrontOfCamera(bSuccess);
				CurrentChild.SetHasValidScreenPosition(CurrentChild.GetInFrontOfCamera() || Indicator->GetClampToScreen());

				if (CurrentChild.HasValidScreenPosition())
				{
					CurrentChild.SetScreenPosition(FVector2D(ScreenPositionWithDepth));
					CurrentChild.SetDepth(ScreenPositionWithDepth.X);
				}

				CurrentChild.SetPriority(Indicator->GetPriority());
				
				{
					IndicatorsChanged |= CurrentChild.IsDirty();
					CurrentChild.ClearDirtyFlag();
				}
			}

			//for循环后
			if (IndicatorsChanged)
			{
				Invalidate(EInvalidateWidgetReason::Paint);
			}
		}
		else // Cond : (LP->GetProjectionData == false)
		{
			SetShowAnyIndicators(false);
		}
	}
	else // Cond : (LP != nullptr)
	{
		SetShowAnyIndicators(false);
	}

	if (AllIndicators.Num() == 0)
	{
		TickHandle.Reset();
		return EActiveTimerReturnType::Stop;
	}

	return EActiveTimerReturnType::Continue;
}

void SActorCanvas::OnIndicatorAdded(UIndicatorDescriptor* Indicator)
{
	AllIndicators.Add(Indicator);
	InactiveIndicator.Add(Indicator);

	AddIndicatorForEntry(Indicator);
}

void SActorCanvas::OnIndicatorRemoved(UIndicatorDescriptor* Indicator)
{
	RemoveIndicatorForEntry(Indicator);

	AllIndicators.Remove(Indicator);
	InactiveIndicator.Remove(Indicator);
}

void SActorCanvas::SetShowAnyIndicators(bool bIndicators)
{
	if (bShowAnyIndicators != bIndicators)
	{
		bShowAnyIndicators = bIndicators;

		if (!bShowAnyIndicators)
		{
			for (int32 ChildIdx = 0; ChildIdx < AllChildren.Num(); ++ChildIdx)
			{
				AllChildren.GetChildAt(ChildIdx)->SetVisibility(EVisibility::Collapsed);
			}
		}
	}
}

void SActorCanvas::AddIndicatorForEntry(UIndicatorDescriptor* Indicator)
{
	TSoftClassPtr<UUserWidget> IndicatorClass = Indicator->GetIndicatorClass();
	if (!IndicatorClass.IsNull())
	{
		TWeakObjectPtr<UIndicatorDescriptor> IndicatorPtr(Indicator);
		AsyncLoad(IndicatorClass, [this, IndicatorPtr, IndicatorClass]()
		{
			if (UIndicatorDescriptor* Indicator = IndicatorPtr.Get())
			{
				if (!AllIndicators.Contains(Indicator))
				{
					return;
				}

				if (UUserWidget* IndicatorWidget = IndicatorPool.GetOrCreateInstance(TSubclassOf<UUserWidget>(IndicatorClass.Get())))
				{
					if (IndicatorWidget->GetClass()->ImplementsInterface(UIndicatorWidgetInterface::StaticClass()))
					{
						IIndicatorWidgetInterface::Execute_BindIndicator(IndicatorWidget, Indicator);
					}

					Indicator->IndicatorWidget = IndicatorWidget;

					InactiveIndicator.Remove(Indicator);

					AddActorSlot(Indicator)
					[
						SAssignNew(Indicator->CanvasHost, SBox)
						[
							IndicatorWidget->TakeWidget()		
						]
					];
				}
			}	
		});

		//After AsyncLoad()
		StartAsyncLoading();
	}
}

void SActorCanvas::RemoveIndicatorForEntry(UIndicatorDescriptor* Indicator)
{
	if (UUserWidget* IndicatorWidget = Indicator->IndicatorWidget.Get())
	{
		if (IndicatorWidget->GetClass()->ImplementsInterface(UIndicatorWidgetInterface::StaticClass()))
		{
			IIndicatorWidgetInterface::Execute_UnbindIndicator(IndicatorWidget, Indicator);
		}

		Indicator->IndicatorWidget = nullptr;

		IndicatorPool.Release(IndicatorWidget);
	}

	TSharedPtr<SWidget> CanvasHost = Indicator->CanvasHost.Pin();
	if (CanvasHost.IsValid())
	{
		RemoveActorSlot(CanvasHost.ToSharedRef());
		Indicator->CanvasHost.Reset();
	}
}

void SActorCanvas::UpdateActiveTimer()
{
	const bool NeedsTicks = AllIndicators.Num() > 0 || !IndicatorCompPtr.IsValid();

	if (NeedsTicks && !TickHandle.IsValid())
	{
		TickHandle = RegisterActiveTimer(0, FWidgetActiveTimerDelegate::CreateSP(this, &SActorCanvas::UpdateCanvas));
	}
}

SActorCanvas::FScopeWidgetSlotArguments SActorCanvas::AddActorSlot(UIndicatorDescriptor* Indicator)
{
	TWeakPtr<SActorCanvas> WeakCanvas = SharedThis(this);
	return FScopeWidgetSlotArguments{ MakeUnique<FSlot>(Indicator), this->CanvasChildren, INDEX_NONE,
		[WeakCanvas](const FSlot*, int32)
		{
			if (TSharedPtr<SActorCanvas> Canvas = WeakCanvas.Pin())
			{
				Canvas->UpdateActiveTimer();
			}
		}};
}

int32 SActorCanvas::RemoveActorSlot(const TSharedRef<SWidget>& SlotWidget)
{
	for (int32 SlotIdx = 0; SlotIdx < CanvasChildren.Num(); ++SlotIdx)
	{
		if (SlotWidget == CanvasChildren[SlotIdx].GetWidget())
		{
			CanvasChildren.RemoveAt(SlotIdx);

			UpdateActiveTimer();

			return SlotIdx;
		}
	}

	return INDEX_NONE;
}
