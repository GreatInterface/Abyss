// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/AbyssPawnExtensionComponent.h"
#include "AbilitySystem/System/AbyssAbilitySystemComponent.h"
#include "Abyss/AbyssGameplayTags.h"
#include "Abyss/AbyssLogChannels.h"
#include "Components/GameFrameworkComponentManager.h"

const FName UAbyssPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UAbyssPawnExtensionComponent::UAbyssPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UAbyssPawnExtensionComponent::SetPawnData(const UAbyssPawnData* InPawnData)
{
	check(InPawnData)

	APawn* Pawn = GetPawnChecked<APawn>();

	if(PawnData)
	{
		return;
	}

	PawnData = InPawnData;

	CheckDefaultInitialization();
}

void UAbyssPawnExtensionComponent::InitializeAbilitySystem(UAbyssAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC && InOwnerActor)

	if(AbilitySystemComponent == InASC)
	{
		//AbilitySystemComponent hasn't Changed
		return;
	}

	if(AbilitySystemComponent)
	{
		//Clear up the old Ability System Component
		UninitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();

	UE_LOG(LogAbyss, Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s]"),*GetNameSafe(InASC), *GetNameSafe(Pawn), *GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));

	if ((ExistingAvatar) && (ExistingAvatar != Pawn))
	{
		if(UAbyssPawnExtensionComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor,Pawn);

	if (ensure(PawnData))
	{
		InASC->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);
	}

	OnAbilitySystemInitialized.Broadcast();
}

void UAbyssPawnExtensionComponent::UninitializeAbilitySystem()
{
	if(!AbilitySystemComponent)
	{
		return;
	}

	if(AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		FGameplayTagContainer AbilityTypesToIgnore{};
		
		AbilitySystemComponent->CancelAbilities(nullptr,&AbilityTypesToIgnore);
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if(AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			AbilitySystemComponent->ClearActorInfo();
		}

		OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}

void UAbyssPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(
	FSimpleMulticastDelegate::FDelegate&& Delegate)
{
	if(!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(MoveTemp(Delegate));
	}

	if(AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void UAbyssPawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate&& Delegate)
{
	if(!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(MoveTemp(Delegate));
	}
}

void UAbyssPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn), TEXT("AbyssPawnExtensionComponent on [%s] can only be added to Pawn actors"), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(UAbyssPawnExtensionComponent::StaticClass(),PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1), TEXT("Only one ExtensionComponent should exist on [%s]"), *GetNameSafe(GetOwner()));

	// Register with the init state system early, this will only work if this is a game world
	RegisterInitStateFeature();
}

void UAbyssPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	BindOnActorInitStateChanged(NAME_None,FGameplayTag(),false);

	ensure(TryToChangeInitState(AbyssGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UAbyssPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();
	UnregisterInitStateFeature();
	
	Super::EndPlay(EndPlayReason);
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool UAbyssPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager)
	using namespace AbyssGameplayTags;
	
	APawn* Pawn = GetPawn<APawn>();
	if(!CurrentState.IsValid() && DesiredState == AbyssGameplayTags::InitState_Spawned)
	{
		if(Pawn)
		{
			return true;
		}
	}
	if(CurrentState == AbyssGameplayTags::InitState_Spawned && DesiredState == AbyssGameplayTags::InitState_DataAvailable)
	{
		if(!PawnData)
		{
			return false;
		}
		if(Pawn->IsLocallyControlled())
		{
			if(!GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}
	if(CurrentState == InitState_DataAvailable && DesiredState == InitState_DataInitialized)
	{
		return Manager->HaveAllFeaturesReachedInitState(Pawn,InitState_DataAvailable);
	}
	if(CurrentState == InitState_DataInitialized && DesiredState == InitState_GameplayReady)
	{
		return true;
	}
	
	return false;
}

void UAbyssPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	// 目前这一切都由监听InitState_DataInit状态变化的其他组件处理
	UE_LOG(LogAbyss, Error, TEXT(" Current[%s],DesiredState[%s]"),*CurrentState.ToString(),*DesiredState.ToString());
}

void UAbyssPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	//如果另一个Feature现在处于 DataAvailable 状态，看看我们是否应该过渡到 DataInitialized 状态
	if(Params.FeatureName != NAME_ActorFeatureName)
	{
		if(Params.FeatureState == AbyssGameplayTags::InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

void UAbyssPawnExtensionComponent::	CheckDefaultInitialization()
{
	CheckDefaultInitializationForImplementers();

	static const TArray<FGameplayTag> StateChain { AbyssGameplayTags::InitState_Spawned, AbyssGameplayTags::InitState_DataAvailable, AbyssGameplayTags::InitState_DataInitialized, AbyssGameplayTags::InitState_GameplayReady };

	ContinueInitStateChain(StateChain);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UAbyssPawnExtensionComponent::HandleControllerChanged()
{
	if(AbilitySystemComponent && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	{
		ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());
		if(AbilitySystemComponent->GetOwnerActor() == nullptr)
		{
			UninitializeAbilitySystem();
		}
		else
		{
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
	}

	CheckDefaultInitialization();
}

void UAbyssPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}
