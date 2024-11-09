// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "AbyssInteractable.h"
#include "Interact/Area/AbyssAreaManagerSubsystem.h"
#include "AbyssInteractable_AreaGate.generated.h"

class UBoxComponent;

UCLASS()
class ABYSSCORERUNTIME_API AAbyssInteractable_AreaGate : public AAbyssInteractable
{
	GENERATED_BODY()

public:

	AAbyssInteractable_AreaGate();

protected:
	
	virtual void BeginPlay() override;

	virtual void Destroyed() override;
	
	UFUNCTION()
	void BeginEnteredOrExitedArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UFUNCTION(BlueprintCallable, Category="Abyss|Interact")
	FAbyssAreaHandle GetHandle() const { return Handle; }	

	
protected:
	
	UPROPERTY(EditAnywhere, Category="Abyss|Intera")
	TObjectPtr<UBoxComponent> AreaCheckBox;
	
	UPROPERTY(EditInstanceOnly, Category="Abyss|Intera")
	FName LevelName = FName();
	
private:
	
	FAbyssAreaHandle Handle;
	
	bool bInArea = false;
};
