// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActionWidget.h"
#include "AbyssActionWidget.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
/**
 * 
 */
UCLASS()
class ABYSS_API UAbyssActionWidget : public UCommonActionWidget
{
	GENERATED_BODY()

public:

	virtual FSlateBrush GetIcon() const override;
	
	//Associated : 相关的
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	const TObjectPtr<UInputAction> AssociatedInputAction;

private:
	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;
};
