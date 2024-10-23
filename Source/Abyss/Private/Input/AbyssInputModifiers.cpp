// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AbyssInputModifiers.h"

#include "EnhancedPlayerInput.h"
#include "Player/AbyssLocalPlayer.h"

namespace AbyssInputModifiersHelpers
{
	static UAbyssLocalPlayer* GetLocalPlayer(const UEnhancedPlayerInput* PlayerInput)
	{
		if(PlayerInput)
		{
			if(APlayerController* PC = Cast<APlayerController>(PlayerInput->GetOuter()))
			{
				return Cast<UAbyssLocalPlayer>(PC->GetLocalPlayer());
			}
		}

		return nullptr;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UAbyssSettingBasedScalar

FInputActionValue UAbyssSettingBasedScalar::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,
	FInputActionValue CurrentValue, float DeltaTime)
{
	if(CurrentValue.GetValueType() != EInputActionValueType::Boolean)
	{
		if(UAbyssLocalPlayer* LocalPlayer = AbyssInputModifiersHelpers::GetLocalPlayer(PlayerInput))
		{
			//TODO : SettingShared

			FVector ScalarToUse = FVector(1.0, 1.0, 1.0);

			return CurrentValue.Get<FVector>() * ScalarToUse;
		}
	}
	
	return CurrentValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////