// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbyssCharacterMovementComponent.generated.h"


USTRUCT(BlueprintType)
struct FAbyssCharacterGroundInfo
{
	GENERATED_BODY()

	FAbyssCharacterGroundInfo()
		: LastUpdateFrame(0)
		, GroundDistance(0.f)
	{}

	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};

UCLASS(Config = Game)
class ABYSS_API UAbyssCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	
	UAbyssCharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Abyss|CharacterMovement")
	const FAbyssCharacterGroundInfo& GetGroundInfo();

private:

	FAbyssCharacterGroundInfo CachedGroundInfo;
	
};
