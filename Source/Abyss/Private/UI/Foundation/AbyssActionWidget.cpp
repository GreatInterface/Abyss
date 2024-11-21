// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/AbyssActionWidget.h"

#include "CommonInputBaseTypes.h"
#include "CommonInputSubsystem.h"
#include "EnhancedInputSubsystems.h"

FSlateBrush UAbyssActionWidget::GetIcon() const
{
	if (AssociatedInputAction)
	{
		if (const UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEnhancedInputSubsystem())
		{
			TArray<FKey> BoundKeys = Subsystem->QueryKeysMappedToAction(AssociatedInputAction);
			FSlateBrush SlateBrush;

			const UCommonInputSubsystem* InputSubsystem = GetInputSubsystem();
			if (!BoundKeys.IsEmpty() && InputSubsystem &&
				UCommonInputPlatformSettings::Get()->TryGetInputBrush(SlateBrush, BoundKeys[0], InputSubsystem->GetCurrentInputType(), InputSubsystem->GetCurrentGamepadName()))
			{
				return SlateBrush;
			}
		}
	}
	
	return Super::GetIcon();
}

UEnhancedInputLocalPlayerSubsystem* UAbyssActionWidget::GetEnhancedInputSubsystem() const
{
	const UWidget* BoundWidget = DisplayedBindingHandle.GetBoundWidget();
	if (const ULocalPlayer* LP = BoundWidget ? BoundWidget->GetOwningLocalPlayer() : GetOwningLocalPlayer())
	{
		return LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	}

	return nullptr;
}
