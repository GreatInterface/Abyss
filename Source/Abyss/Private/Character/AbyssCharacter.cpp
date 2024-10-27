// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AbyssCharacter.h"
#include "AbilitySystem/System/AbyssAbilitySystemComponent.h"
#include "Camera/AbyssCameraComponent.h"
#include "Camera/AbyssSpringArmComponent.h"
#include "Character/Component/AbyssCharacterMovementComponent.h"
#include "Character/Component/AbyssPawnExtensionComponent.h"
#include "Player/AbyssPlayerController.h"
#include "Player/AbyssPlayerState.h"

AAbyssCharacter::AAbyssCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UAbyssCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	PawnExtComponent = CreateDefaultSubobject<UAbyssPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this,&ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this,&ThisClass::OnAbilitySystemUninitialized));

	CameraBoom = CreateDefaultSubobject<UAbyssSpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UAbyssCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraBoom);
	CameraComponent->bUsePawnControlRotation = false;
}

AAbyssPlayerController* AAbyssCharacter::GetAbyssPlayerController() const
{
	return CastChecked<AAbyssPlayerController>(Controller,ECastCheckedType::NullAllowed);
}

AAbyssPlayerState* AAbyssCharacter::GetAbyssPlayerState() const
{
	return CastChecked<AAbyssPlayerState>(GetPlayerState(),ECastCheckedType::NullAllowed);
}

UAbyssAbilitySystemComponent* AAbyssCharacter::GetAbyssAbilitySystemComponent() const
{
	return Cast<UAbyssAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* AAbyssCharacter::GetAbilitySystemComponent() const
{
	if(PawnExtComponent == nullptr)
	{
		return nullptr;
	}
	
	return PawnExtComponent->GetAbyssAbilitySystemComponent();
}

void AAbyssCharacter::ToggleCrouch()
{
	const UAbyssCharacterMovementComponent* MoveComp = CastChecked<UAbyssCharacterMovementComponent>(GetCharacterMovement());

	if(bIsCrouched || MoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if(MoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

void AAbyssCharacter::OnAbilitySystemInitialized()
{
	UAbyssAbilitySystemComponent* AbyssASC = GetAbyssAbilitySystemComponent();
	check(AbyssASC);

	//TODO : Lyra中为 HealthComponent 进行初始化，随着项目推进，可能需要为自定义Component进行初始化
}

void AAbyssCharacter::OnAbilitySystemUninitialized()
{
	//TODO : @See OnAbilitySystemInitialized -> TODO , Uninit Component;
}

void AAbyssCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PawnExtComponent->HandleControllerChanged();
}

void AAbyssCharacter::UnPossessed()
{
	Super::UnPossessed();

	PawnExtComponent->HandleControllerChanged();
}

void AAbyssCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}
