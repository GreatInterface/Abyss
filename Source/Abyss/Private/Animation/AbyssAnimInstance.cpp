// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AbyssAnimInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Character/AbyssCharacter.h"
#include "Character/Component/AbyssCharacterMovementComponent.h"

UAbyssAnimInstance::UAbyssAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAbyssAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC)

	GameplayTagPropertyMap.Initialize(this, ASC);	
}

void UAbyssAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if(AActor* Actor = GetOwningActor())
	{
		if(UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}
}

void UAbyssAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(const AAbyssCharacter* Character = Cast<AAbyssCharacter>(GetOwningActor()))
	{
		UAbyssCharacterMovementComponent* CharMoveComp = CastChecked<UAbyssCharacterMovementComponent>(Character->GetCharacterMovement());
		const FAbyssCharacterGroundInfo& GroundInfo = CharMoveComp->GetGroundInfo();
		GroundDistance = GroundInfo.GroundDistance;
	}
}
