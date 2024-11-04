// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Subsystem/AbyssUIManagerSubsystem.h"

#include "CommonLocalPlayer.h"
#include "GameUIPolicy.h"
#include "PrimaryGameLayout.h"
#include "GameFramework/HUD.h"


UAbyssUIManagerSubsystem::UAbyssUIManagerSubsystem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAbyssUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::Tick), 0.0f);
}

void UAbyssUIManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
}

bool UAbyssUIManagerSubsystem::Tick(float DeltaTime)
{
	SyncRootLayoutVisibilityToShowHUD();

	return true;
}

void UAbyssUIManagerSubsystem::SyncRootLayoutVisibilityToShowHUD()
{
	if (const UGameUIPolicy* Policy = GetCurrentUIPolicy())
	{
		for (const ULocalPlayer* LP : GetGameInstance()->GetLocalPlayers())
		{
			bool bShouldShowUI = true;

			if (const APlayerController* PC = LP->GetPlayerController(GetWorld()))
			{
				const AHUD* HUD = PC->GetHUD();

				if (HUD && !HUD->bShowHUD)
				{
					bShouldShowUI = false;
				}
			}

			if (UPrimaryGameLayout* RootLayout = Policy->GetRootLayout(CastChecked<UCommonLocalPlayer>(LP)))
			{
				const ESlateVisibility Visibility = bShouldShowUI ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;
				if (Visibility != RootLayout->GetVisibility())
				{
					RootLayout->SetVisibility(Visibility);
				}
			}
		}
	}
}
