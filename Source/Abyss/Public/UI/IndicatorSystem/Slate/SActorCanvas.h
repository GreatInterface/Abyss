#pragma once
#include "AsyncMixin.h"
#include "Blueprint/UserWidgetPool.h"

struct FUserWidgetPool;
class UAbyssIndicatorManagerComponent;
class UIndicatorDescriptor;

class SActorCanvas : public SPanel, public FAsyncMixin, public FGCObject
{
public:
	class FSlot : public TSlotBase<FSlot>
	{
	public:
		FSlot(UIndicatorDescriptor* InIndicator)
			: TSlotBase()
			, Indicator(InIndicator)
			, ScreenPosition(FVector2D::ZeroVector)
			, Depth(0)
			, Priority(0.f)
			, bIsIndicatorVisible(true)
			, bInFrontOfCamera(true)
			, bHasValidScreenPosition(false)
			, bDirty(true)
			, bWasIndicatorClamped(false)
			, bWasIndicatorClampedStatusChanged(false)
		{}

		SLATE_SLOT_BEGIN_ARGS(FSlot, TSlotBase<FSlot>)
		SLATE_SLOT_END_ARGS()
		using TSlotBase<FSlot>::Construct;

		FVector2D GetScreenPosition() const { return ScreenPosition; }
		void SetScreenPosition(FVector2D InScreenPosition)
		{
			if (ScreenPosition != InScreenPosition)
			{
				ScreenPosition = InScreenPosition;
				bDirty = true;
			}
		}

		double GetDepth() const { return Depth; }
		void SetDepth(double InDepth)
		{
			if (Depth != InDepth)
			{
				Depth = InDepth;
				bDirty = true;
			}
		}

		int32 GetPriority() const { return Priority; }
		void SetPriority(int32 InPriority)
		{
			if (Priority != InPriority)
			{
				Priority = InPriority;
				bDirty = true;
			}
		} 
		
		bool GetIsIndicatorVisible() const { return bIsIndicatorVisible; }
		void SetIsIndicatorVisible(bool bVisible)
		{
			if (bIsIndicatorVisible != bVisible)
			{
				bIsIndicatorVisible = bVisible;
				bDirty = true;
			}

			RefreshVisibility();
		}

		bool GetInFrontOfCamera() const { return bInFrontOfCamera; }
		void SetInFrontOfCamera(bool bInFront)
		{
			if (bInFrontOfCamera != bInFront)
			{
				bInFrontOfCamera = bInFront;
				bDirty = true;
			}

			RefreshVisibility();
		}

		bool HasValidScreenPosition() const { return bHasValidScreenPosition; }
		void SetHasValidScreenPosition(bool bValidScreenPosition)
		{
			if (bHasValidScreenPosition != bValidScreenPosition)
			{
				bHasValidScreenPosition = bValidScreenPosition;
				bDirty = true;
			}

			RefreshVisibility();
		}

		bool IsDirty() const { return bDirty; }

		void ClearDirtyFlag() { bDirty = false; }

		bool WasIndicatorClamped() const { return bWasIndicatorClamped; }
		void SetWasIndicatorClamped(bool bWasClamped) const
		{
			if (bWasClamped != bWasIndicatorClamped)
			{
				bWasIndicatorClamped = bWasClamped;
				bWasIndicatorClampedStatusChanged = true;
			}
		}

		bool WasIndicatorClampedStatusChanged() const { return bWasIndicatorClampedStatusChanged; }
		void ClearIndicatorClampedStatusChangedFlag()
		{
			bWasIndicatorClampedStatusChanged = false;
		}
	
	private:
		void RefreshVisibility()
		{
			const bool bIsVisible = bIsIndicatorVisible && bHasValidScreenPosition;
			GetWidget()->SetVisibility(bIsVisible ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed);
		}

		UIndicatorDescriptor* Indicator;
		FVector2D ScreenPosition;
		double Depth;
		int32 Priority;

		bool bIsIndicatorVisible;
		bool bInFrontOfCamera;
		bool bHasValidScreenPosition;
		bool bDirty;

		mutable bool bWasIndicatorClamped;
		mutable bool bWasIndicatorClampedStatusChanged;

		friend class SActorCanvas;
	};

	class FArrowSlot : public TSlotBase<FArrowSlot>{};

	SLATE_BEGIN_ARGS(SActorCanvas)
	{
		_Visibility = EVisibility::HitTestInvisible;
	}

		SLATE_SLOT_ARGUMENT(SActorCanvas::FSlot, Slots)
		
	SLATE_END_ARGS()

	SActorCanvas()
		: CanvasChildren(this)
		, ArrowChildren(this)
		, AllChildren(this)
	{
		AllChildren.AddChildren(CanvasChildren);
		AllChildren.AddChildren(ArrowChildren);
	}

	void Construct(const FArguments& InArgs, const FLocalPlayerContext& InCtx, const FSlateBrush* InActorCanvasArrowBrush);

private:
	EActiveTimerReturnType UpdateCanvas(double InCurrentTime, float InDeltaTime);

	void OnIndicatorAdded(UIndicatorDescriptor* Indicator);
	void OnIndicatorRemoved(UIndicatorDescriptor* Indicator);

	void SetShowAnyIndicators(bool bIndicators);
	
	void AddIndicatorForEntry(UIndicatorDescriptor* Indicator);
	void RemoveIndicatorForEntry(UIndicatorDescriptor* Indicator);
	
	void UpdateActiveTimer();

	using FScopeWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;
	FScopeWidgetSlotArguments AddActorSlot(UIndicatorDescriptor* Indicator);
	int32 RemoveActorSlot(const TSharedRef<SWidget>& SlotWidget);

private:
	TArray<TObjectPtr<UIndicatorDescriptor>> AllIndicators;
	TArray<UIndicatorDescriptor*> InactiveIndicator;

	FLocalPlayerContext LocalPlayerContext;
	TWeakObjectPtr<UAbyssIndicatorManagerComponent> IndicatorCompPtr;

	TPanelChildren<FSlot> CanvasChildren;
	mutable TPanelChildren<FArrowSlot> ArrowChildren;
	FCombinedChildren AllChildren;

	FUserWidgetPool IndicatorPool;

	const FSlateBrush* ActorCanvasArrowBrush = nullptr;

	mutable int32 NextArrowIndex = INDEX_NONE;
	mutable int32 ArrowIndexLastUpdate = 0;

	bool bDrawElementsInOrder = false;

	bool bShowAnyIndicators = false;

	mutable TOptional<FGeometry> OptionalPaintGeometry;

	TSharedPtr<FActiveTimerHandle> TickHandle;
};


class SActorCanvasArrowWidget : public SLeafWidget
{
	SLATE_BEGIN_ARGS(SActorCanvasArrowWidget)
		{}
		
	SLATE_END_ARGS()

	SActorCanvasArrowWidget()
		: Rotation(0.f)
		, Arrow(nullptr)
	{}

	void Construct(const FArguments& InArgs, const FSlateBrush* ActorCanvasArrowBrush)
	{
		Arrow = ActorCanvasArrowBrush;
		SetCanTick(false);
	}

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	                      FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
	                      bool bParentEnabled) const override
	{
		int32 MaxLayerId = LayerId;

		if (Arrow)
		{
			const bool bIsEnabled = ShouldBeEnabled(bParentEnabled);
			const ESlateDrawEffect DrawEffect = bIsEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
			const FColor FinalColorAndOpacity = (InWidgetStyle.GetColorAndOpacityTint() * Arrow->GetTint(InWidgetStyle)).ToFColor(true);

			FSlateDrawElement::MakeRotatedBox(
				OutDrawElements,
				MaxLayerId++,
				AllottedGeometry.ToPaintGeometry(Arrow->ImageSize, FSlateLayoutTransform()),
				Arrow,
				DrawEffect,
				FMath::DegreesToRadians(GetRotation()),
				TOptional<FVector2D>(),
				FSlateDrawElement::RelativeToElement,
				FinalColorAndOpacity
				);
		}

		return MaxLayerId;
	}

	float GetRotation() const { return Rotation; }
	void SetRotation(float InRotation)
	{
		Rotation = FMath::Fmod(InRotation, 360.f);
	}

	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override
	{
		if (Arrow)
		{
			return Arrow->ImageSize;
		}

		return FVector2D::ZeroVector;
	}
	
private:
	float Rotation;

	const FSlateBrush* Arrow;
};