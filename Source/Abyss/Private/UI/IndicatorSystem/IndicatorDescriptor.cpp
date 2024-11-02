﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/IndicatorSystem/IndicatorDescriptor.h"
#include "UI/IndicatorSystem/AbyssIndicatorManagerComponent.h"

bool FIndicatorProjection::Project(const UIndicatorDescriptor& IndicatorDescriptor,
	const FSceneViewProjectionData& InProjectionData, const FVector2f& ScreenSize, FVector& OutScreenPositionWithDepth)
{
	if (USceneComponent* Component = IndicatorDescriptor.GetSceneComponent())
	{
		TOptional<FVector> WorldLocation;
		if (IndicatorDescriptor.GetComponentSocketName() != NAME_None)
		{
			WorldLocation = Component->GetSocketLocation(IndicatorDescriptor.GetComponentSocketName());
		}
		else
		{
			WorldLocation = Component->GetComponentLocation();
		}

		const FVector ProjectWorldLocation = WorldLocation.GetValue() + IndicatorDescriptor.WorldPositionOffset;
		
		const EActorCanvasProjectionMode ProjectionMode = IndicatorDescriptor.ProjectionMode;
		switch (ProjectionMode)
		{
		case EActorCanvasProjectionMode::ComponentPoint:
			if (WorldLocation.IsSet())
			{
				FVector2D OutScreenSpacePosition;
				const bool bInFrontOfCamera = ULocalPlayer::GetPixelPoint(InProjectionData, ProjectWorldLocation, OutScreenSpacePosition, &ScreenSize);

				OutScreenSpacePosition.X += IndicatorDescriptor.ScreenSpaceOffset.X * (bInFrontOfCamera ? 1 : -1);
				OutScreenSpacePosition.Y += IndicatorDescriptor.ScreenSpaceOffset.Y;

				if (!bInFrontOfCamera && FBox2f(FVector2f::Zero(), ScreenSize).IsInside((FVector2f)OutScreenSpacePosition))
				{
					const FVector2f CenterToPosition = (FVector2f(OutScreenSpacePosition) - (ScreenSize/2)).GetSafeNormal();
					OutScreenSpacePosition = FVector2D((ScreenSize / 2) + CenterToPosition * ScreenSize);
				}

				OutScreenPositionWithDepth = FVector(OutScreenSpacePosition.X, OutScreenSpacePosition.Y, FVector::Dist(InProjectionData.ViewOrigin, ProjectWorldLocation));

				return true;
			}

			return false;
			
		case EActorCanvasProjectionMode::ComponentScreenBoundingBox:
		case EActorCanvasProjectionMode::ActorScreenBoundingBox:
		{
			FBox IndicatorBox;
			if (ProjectionMode == EActorCanvasProjectionMode::ActorScreenBoundingBox)
			{
				IndicatorBox = Component->GetOwner()->GetComponentsBoundingBox();
			}
			else
			{
				IndicatorBox = Component->Bounds.GetBox();
			}

			FVector2D LL, UR;
			const bool bInFrontOfCamera = ULocalPlayer::GetPixelBoundingBox(InProjectionData, IndicatorBox, LL, UR, &ScreenSize);

			const FVector& BoundingBoxAnchor = IndicatorDescriptor.BoundingBoxAnchor;
			const FVector2D& ScreenSpaceOffset = IndicatorDescriptor.ScreenSpaceOffset;

			FVector ScreenPositionWithDepth;
			ScreenPositionWithDepth.X = FMath::Lerp(LL.X, UR.X, BoundingBoxAnchor.X) + ScreenSpaceOffset.X * (bInFrontOfCamera ? 1 : -1);
			ScreenPositionWithDepth.Y = FMath::Lerp(LL.Y, UR.Y, BoundingBoxAnchor.Y) + ScreenSpaceOffset.Y;
			ScreenPositionWithDepth.Z = FVector::Dist(InProjectionData.ViewOrigin, ProjectWorldLocation);

			const FVector2f ScreenSpacePosition = FVector2f(FVector2D(ScreenPositionWithDepth));
			if (!bInFrontOfCamera && FBox2f(FVector2f::Zero(), ScreenSize).IsInside(ScreenSpacePosition))
			{
				const FVector2f CenterToPosition = (ScreenSpacePosition - (ScreenSize / 2)).GetSafeNormal();
				const FVector2f ScreenPositionFromBehind = (ScreenSize / 2) + CenterToPosition * ScreenSize;
				ScreenPositionWithDepth.X = ScreenPositionFromBehind.X;
				ScreenPositionWithDepth.Y = ScreenPositionFromBehind.Y;
			}
				
			OutScreenPositionWithDepth = ScreenPositionWithDepth;
				
			return true;
		}
		case EActorCanvasProjectionMode::ActorBoundingBox:
		case EActorCanvasProjectionMode::ComponentBoundingBox:
			
			FBox IndicatorBox;
			if (ProjectionMode == EActorCanvasProjectionMode::ActorBoundingBox)
			{
				IndicatorBox = Component->GetOwner()->GetComponentsBoundingBox();
			}
			else
			{
				IndicatorBox = Component->Bounds.GetBox();
			}

			const FVector ProjectBoxPoint = IndicatorBox.GetCenter() + (IndicatorBox.GetSize() * (IndicatorDescriptor.BoundingBoxAnchor - FVector(0.5)));

			FVector2D OutScreenSpacePosition;
			const bool bInFrontOfCamera = ULocalPlayer::GetPixelPoint(InProjectionData, ProjectBoxPoint, OutScreenSpacePosition, &ScreenSize);
			OutScreenSpacePosition.X += IndicatorDescriptor.ScreenSpaceOffset.X * (bInFrontOfCamera ? 1 : -1);
			OutScreenSpacePosition.Y += IndicatorDescriptor.ScreenSpaceOffset.Y;

			if (!bInFrontOfCamera && FBox2f(FVector2f::Zero(), ScreenSize).IsInside((FVector2f)OutScreenSpacePosition))
			{
				const FVector2f CenterToPosition = (FVector2f(OutScreenSpacePosition) - (ScreenSize / 2)).GetSafeNormal();
				OutScreenSpacePosition = FVector2D((ScreenSize / 2) + CenterToPosition * ScreenSize);
			}

			OutScreenPositionWithDepth = FVector(OutScreenSpacePosition.X, OutScreenSpacePosition.Y, FVector::Dist(InProjectionData.ViewOrigin, ProjectBoxPoint));
					
			return true;
		}
	}
	
	return false;
}

void UIndicatorDescriptor::SetManagerComponent(UAbyssIndicatorManagerComponent* InManager)
{
	if(ensure(ManagerPtr.IsExplicitlyNull()))
	{
		ManagerPtr = InManager;
	}
}
