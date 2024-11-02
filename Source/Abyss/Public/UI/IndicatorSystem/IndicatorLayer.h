// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "IndicatorLayer.generated.h"

class SActorCanvas;
/**
 * 
 */
UCLASS()
class ABYSS_API UIndicatorLayer : public UWidget
{
	GENERATED_BODY()

public:
	UIndicatorLayer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FSlateBrush ArrowBrush;

protected:

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual TSharedRef<SWidget> RebuildWidget() override;
	
protected:
	
	TSharedPtr<SActorCanvas> MyActorCanvas;
};
