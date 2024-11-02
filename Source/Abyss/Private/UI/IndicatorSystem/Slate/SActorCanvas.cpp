#include "UI/IndicatorSystem/Slate/SActorCanvas.h"
#include "UI/IndicatorSystem/AbyssIndicatorManagerComponent.h"
#include "UI/IndicatorSystem/Interface/IndicatorWidgetInterface.h"

namespace EArrowDirection
{
	enum Type
	{
		Left,
		Top,
		Right,
		Bottom,
		MAX
	};
}

// Angles for the direction of the arrow to display
const float ArrowRotations[EArrowDirection::MAX] =
{
	270.0f,
	0.0f,
	90.0f,
	180.0f
};

// Offsets for the each direction that the arrow can point
const FVector2D ArrowOffsets[EArrowDirection::MAX] =
{
	FVector2D(-1.0f, 0.0f),
	FVector2D(0.0f, -1.0f),
	FVector2D(1.0f, 0.0f),
	FVector2D(0.0f, 1.0f)
};


/** ---------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------- */
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

void SActorCanvas::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_SActorCanvas_OnArrangeChildren);

	NextArrowIndex = 0;

	if (bShowAnyIndicators)
	{
		const FVector2D ArrowWidgetSize = ActorCanvasArrowBrush->GetImageSize();
		const FIntPoint FixedPadding = FIntPoint(10.f, 10.f) + FIntPoint(ArrowWidgetSize.X, ArrowWidgetSize.Y);
		const FVector Center = FVector(AllottedGeometry.Size * 0.5f, 0.f);

		TArray<const SActorCanvas::FSlot*> SortedSlots;
		for (int32 ChildIndex = 0; ChildIndex < CanvasChildren.Num(); ++ChildIndex)
		{
			SortedSlots.Add(&CanvasChildren[ChildIndex]);
		}

		SortedSlots.StableSort([](const FSlot& A, const FSlot& B)
		{
			return A.GetPriority() == B.GetPriority() ? (A.GetDepth() > B.GetDepth()) : (A.GetPriority() < B.GetPriority()); 
		});

		for (int32 ChildIndex = 0; ChildIndex < SortedSlots.Num(); ++ChildIndex)
		{
			//grab a child
			const SActorCanvas::FSlot& CurChild = *SortedSlots[ChildIndex];
			const UIndicatorDescriptor* Indicator = CurChild.Indicator;

			// Skip this indicator if it's invalid or has an invalid world position
			if (!ArrangedChildren.Accepts(CurChild.GetWidget()->GetVisibility()))
			{
				CurChild.SetWasIndicatorClamped(false);
				continue;
			}

			FVector2D ScreenPosition = CurChild.GetScreenPosition();
			const bool bInFrontOfCamera = CurChild.GetInFrontOfCamera();

			// Don't bother if we can't project the position and the indicator doesn't want to be clamped
			const bool bShouldClamp = Indicator->GetClampToScreen();

			//get the offset and final size of the slot
			FVector2D SlotSize, SlotOffset, SlotPaddingMin, SlotPaddingMax;
			GetOffsetAndSize(Indicator, SlotSize, SlotOffset, SlotPaddingMin, SlotPaddingMax);

			bool bWasIndicatorClamped = false;

			// If we don't have to clamp this thing, we can skip a lot of work
			if (bShouldClamp)
			{
				//figure out if we clamped to any edge of the screen
				EArrowDirection::Type ClampDir = EArrowDirection::MAX;

				// Determine the size of inner screen rect to clamp within
				const FIntPoint RectMin = FIntPoint(SlotPaddingMin.X, SlotPaddingMin.Y) + FixedPadding;
				const FIntPoint RectMax = FIntPoint(AllottedGeometry.Size.X - SlotPaddingMax.X, AllottedGeometry.Size.Y - SlotPaddingMax.Y) - FixedPadding;
				const FIntRect ClampRect(RectMin, RectMax);

				// Make sure the screen position is within the clamp rect
				if (!ClampRect.Contains(FIntPoint(ScreenPosition.X, ScreenPosition.Y)))
				{
					const FPlane Planes[] =
					{
						FPlane(FVector(1.0f, 0.0f, 0.0f), ClampRect.Min.X),	// Left
						FPlane(FVector(0.0f, 1.0f, 0.0f), ClampRect.Min.Y),	// Top
						FPlane(FVector(-1.0f, 0.0f, 0.0f), -ClampRect.Max.X),	// Right
						FPlane(FVector(0.0f, -1.0f, 0.0f), -ClampRect.Max.Y)	// Bottom
					};

					for (int32 i = 0; i < EArrowDirection::MAX; ++i)
					{
						FVector NewPoint;
						if (FMath::SegmentPlaneIntersection(Center, FVector(ScreenPosition, 0.0f), Planes[i], NewPoint))
						{
							ClampDir = (EArrowDirection::Type)i;
							ScreenPosition = FVector2D(NewPoint);
						}
					}
				}
				else if (!bInFrontOfCamera)
				{
					const float ScreenXNorm = ScreenPosition.X / (RectMax.X - RectMin.X);
					const float ScreenYNorm = ScreenPosition.Y / (RectMax.Y - RectMin.Y);
					//we need to pin this thing to the side of the screen
					if (ScreenXNorm < ScreenYNorm)
					{
						if (ScreenXNorm < (-ScreenYNorm + 1.0f))
						{
							ClampDir = EArrowDirection::Left;
							ScreenPosition.X = ClampRect.Min.X;
						}
						else
						{
							ClampDir = EArrowDirection::Bottom;
							ScreenPosition.Y = ClampRect.Max.Y;
						}
					}
					else
					{
						if (ScreenXNorm < (-ScreenYNorm + 1.0f))
						{
							ClampDir = EArrowDirection::Top;
							ScreenPosition.Y = ClampRect.Min.Y;
						}
						else
						{
							ClampDir = EArrowDirection::Right;
							ScreenPosition.X = ClampRect.Max.X;
						}
					}
				}

				bWasIndicatorClamped = (ClampDir != EArrowDirection::MAX);

				// should we show an arrow
				if (Indicator->bShowClampToScreenArrow &&
					bWasIndicatorClamped &&
					ArrowChildren.IsValidIndex(NextArrowIndex))
				{
					const FVector2D ArrowOffsetDirection = ArrowOffsets[ClampDir];
					const float ArrowRotation = ArrowRotations[ClampDir];

					//grab an arrow widget
					TSharedRef<SActorCanvasArrowWidget> ArrowWidgetToUse = StaticCastSharedRef<SActorCanvasArrowWidget>(ArrowChildren.GetChildAt(NextArrowIndex));
					NextArrowIndex++;

					//set the rotation of the arrow
					ArrowWidgetToUse->SetRotation(ArrowRotation);

					//figure out the magnitude of the offset
					const FVector2D OffsetMagnitude = (SlotSize + ArrowWidgetSize) * 0.5f;

					//used to center the arrow on the position
					const FVector2D ArrowCenteringOffset = -(ArrowWidgetSize * 0.5f);

					FVector2D ArrowAlignmentOffset = FVector2D::ZeroVector;
					switch (Indicator->VAlignment)
					{
					case VAlign_Top:
						ArrowAlignmentOffset = SlotSize * FVector2D(0.0f, 0.5f);
						break;
					case VAlign_Bottom:
						ArrowAlignmentOffset = SlotSize * FVector2D(0.0f, -0.5f);
						break;
					}

					//figure out the offset for the arrow
					const FVector2D WidgetOffset = (OffsetMagnitude * ArrowOffsetDirection);

					const FVector2D FinalOffset = (WidgetOffset + ArrowAlignmentOffset + ArrowCenteringOffset);

					//get the final position
					const FVector2D FinalPosition = (ScreenPosition + FinalOffset);

					ArrowWidgetToUse->SetVisibility(EVisibility::HitTestInvisible);

					// Inject the arrow on top of the indicator
					ArrangedChildren.AddWidget(AllottedGeometry.MakeChild(
						ArrowWidgetToUse,			// The child widget being arranged
						FinalPosition,				// Child's local position (i.e. position within parent)
						ArrowWidgetSize,			// Child's size
						1.f							// Child's scale
					));
				}
			}

			CurChild.SetWasIndicatorClamped(bWasIndicatorClamped);

			// Add the information about this child to the output list (ArrangedChildren)
			ArrangedChildren.AddWidget(AllottedGeometry.MakeChild(
				CurChild.GetWidget(),
				ScreenPosition + SlotOffset,
				SlotSize,
				1.f
			));
		}
	}
}

int32 SActorCanvas::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_SActorCanvas_OnPaint);

	OptionalPaintGeometry = AllottedGeometry;

	FArrangedChildren ArrangedChildren(EVisibility::Visible);
	ArrangeChildren(AllottedGeometry, ArrangedChildren);

	int32 MaxLayerId = LayerId;

	const FPaintArgs NewArgs = Args.WithNewParent(this);
	const bool bShouldBeEnabled = ShouldBeEnabled(bParentEnabled);

	for (const FArrangedWidget& CurWidget : ArrangedChildren.GetInternalArray())
	{
		if (!IsChildWidgetCulled(MyCullingRect, CurWidget))
		{
			SWidget* MutableWidget = const_cast<SWidget*>(&CurWidget.Widget.Get());

			const int32 CurWidgetsMaxLayerId = CurWidget.Widget->Paint(NewArgs, CurWidget.Geometry, MyCullingRect, OutDrawElements, bDrawElementsInOrder ? MaxLayerId : LayerId, InWidgetStyle, bShouldBeEnabled);
			MaxLayerId = FMath::Max(MaxLayerId, CurWidgetsMaxLayerId);
		}
		else
		{
			//SlateGI - RemoveContent
		}
	}

	return MaxLayerId;
}

FString SActorCanvas::GetReferencerName() const
{
	return TEXT("SActorCanvas");
}

void SActorCanvas::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObjects(AllIndicators);
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

void SActorCanvas::GetOffsetAndSize(const UIndicatorDescriptor* Indicator, FVector2D& OutSize, FVector2D& OutOffset,
	FVector2D& OutPaddingMin, FVector2D& OutPaddingMax) const
{
	//This might get used one day
	FVector2D AllottedSize = FVector2D::ZeroVector;

	//grab the desired size of the child widget
	TSharedPtr<SWidget> CanvasHost = Indicator->CanvasHost.Pin();
	if (CanvasHost.IsValid())
	{
		OutSize = CanvasHost->GetDesiredSize();
	}

	//handle horizontal alignment
	switch(Indicator->HAlignment)
	{
	case HAlign_Left: // same as Align_Top
		OutOffset.X = 0.0f;
		OutPaddingMin.X = 0.0f;
		OutPaddingMax.X = OutSize.X;
		break;
		
	case HAlign_Center:
		OutOffset.X = (AllottedSize.X - OutSize.X) / 2.0f;
		OutPaddingMin.X = OutSize.X / 2.0f;
		OutPaddingMax.X = OutPaddingMin.X;
		break;
		
	case HAlign_Right: // same as Align_Bottom
		OutOffset.X = AllottedSize.X - OutSize.X;
		OutPaddingMin.X = OutSize.X;
		OutPaddingMax.X = 0.0f;
		break;
	}

	//Now, handle vertical alignment
	switch(Indicator->VAlignment)
	{
	case VAlign_Top:
		OutOffset.Y = 0.0f;
		OutPaddingMin.Y = 0.0f;
		OutPaddingMax.Y = OutSize.Y;
		break;
		
	case VAlign_Center:
		OutOffset.Y = (AllottedSize.Y - OutSize.Y) / 2.0f;
		OutPaddingMin.Y = OutSize.Y / 2.0f;
		OutPaddingMax.Y = OutPaddingMin.Y;
		break;
		
	case VAlign_Bottom:
		OutOffset.Y = AllottedSize.Y - OutSize.Y;
		OutPaddingMin.Y = OutSize.Y;
		OutPaddingMax.Y = 0.0f;
		break;
	}
}
