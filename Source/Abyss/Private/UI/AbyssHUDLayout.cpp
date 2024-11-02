// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AbyssHUDLayout.h"
#include "CommonUIExtensions.h"
#include "NativeGameplayTags.h"
#include "Input/CommonUIInputTypes.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_MENU, "UI.Layer.Menu")
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_ACTION_ESCAPE, "UI.Action.Escape")

UAbyssHUDLayout::UAbyssHUDLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAbyssHUDLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//任何 UWidget 都可以直接通过 FCommonUIActionRouter 进行注册，不过一般情况下并不需要。
	//持久绑定在注册时始终监听输入，而普通绑定只有在该 widget 的所有可激活父对象都被激活时才会监听
	RegisterUIActionBinding(
		FBindUIActionArgs(
			FUIActionTag::ConvertChecked(TAG_UI_ACTION_ESCAPE),
			false,
			FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEscapeAction))
	);
}

void UAbyssHUDLayout::HandleEscapeAction()
{
	if (ensure(!EscapeMenuClass.IsNull()))
	{
		UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(GetOwningLocalPlayer(), TAG_UI_LAYER_MENU, EscapeMenuClass);
	}
}
