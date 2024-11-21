// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "AbyssButtonBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class ABYSS_API UAbyssButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void SetButtonText(const FText& InText);

protected:

	virtual void NativePreConstruct() override;

	virtual void UpdateInputActionWidget() override;
	virtual void OnInputMethodChanged(ECommonInputType CurrentInputType) override;

	void RefreshButtonText();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonText(const FText& InText);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonStyle();
	
private:
	UPROPERTY(EditAnywhere, Category=Button, meta=(InlineEditConditionToggle))
	bool bOverride_ButtonText = false;

	UPROPERTY(EditAnywhere, Category=Button, meta=(editcondition="bOverride_ButtonText"))
	FText ButtonText;
};
