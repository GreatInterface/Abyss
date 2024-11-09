// Fill out your copyright notice in the Description page of Project Settings.

#include "Interact/AbyssInteractable_AreaGate.h"
#include "Components/BoxComponent.h"
#include "Interact/Area/AbyssAreaManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"

AAbyssInteractable_AreaGate::AAbyssInteractable_AreaGate()
{
	PrimaryActorTick.bCanEverTick = false;

	//分配一个AreaHandle
	Handle.GenerateNewHandle();

	AreaCheckBox = CreateDefaultSubobject<UBoxComponent>("AreaCheckBoxComp");
}

void AAbyssInteractable_AreaGate::BeginPlay()
{
	Super::BeginPlay();

	AreaCheckBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::BeginEnteredOrExitedArea);
}

void AAbyssInteractable_AreaGate::Destroyed()
{
	AreaCheckBox->OnComponentBeginOverlap.RemoveAll(this);
	
	Super::Destroyed();
}


void AAbyssInteractable_AreaGate::BeginEnteredOrExitedArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (UWorld* World = GetWorld())
	{
		if (UAbyssAreaManagerSubsystem* Subsystem = World->GetSubsystem<UAbyssAreaManagerSubsystem>())
		{
			//进入关卡区域
			if (!bInArea && Subsystem->OnEnteredArea.IsBound())
			{
				FLatentActionInfo ActionInfo;
				ActionInfo.CallbackTarget = this;
		
				UGameplayStatics::LoadStreamLevel(World, LevelName, true, false, ActionInfo);
				
				Subsystem->OnEnteredArea.Broadcast(Handle);
				bInArea = true;
			}
			else if (bInArea && Subsystem->OnExitedArea.IsBound())
			{
				FLatentActionInfo ActionInfo;
				ActionInfo.CallbackTarget = this;
		
				UGameplayStatics::UnloadStreamLevel(World, LevelName, ActionInfo, false);

				Subsystem->OnExitedArea.Broadcast(Handle);
				bInArea = false;
			}
		}
	}
}


