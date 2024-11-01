// Fill out your copyright notice in the Description page of Project Settings.

#include "Interact/Ability/AbyssGameplayAbility_Interact.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "Interact/InteractableTarget.h"
#include "Interact/InteractionOption.h"
#include "Interact/InteractionLibrary.h"
#include "Interact/Task/AbilityTask_GrantNearbyInteraction.h"
#include "Player/AbyssPlayerController.h"
#include "UI/IndicatorSystem/AbyssIndicatorManagerComponent.h"
#include "UI/IndicatorSystem/IndicatorDescriptor.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Ability_Interaction_Activate, "Ability.Interaction.Activate")
/** Declare in AbyssInteractDurationMessage.h */
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_INTENACTION_DURATION_MESSAGE, "Ability.Interaction.Duration.Message")

UAbyssGameplayAbility_Interact::UAbyssGameplayAbility_Interact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EAbyssAbilityActivationPolicy::OnSpawn;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UAbyssGameplayAbility_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if(ASC && ASC->GetOwnerRole() == ROLE_Authority)
	{
		UAbilityTask_GrantNearbyInteraction* Task = UAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractors(this, InteractionScanRange, InteractionScanRate);
		Task->ReadyForActivation();
	}
}

void UAbyssGameplayAbility_Interact::UpdateInteractions(const TArray<FInteractionOption>& InteractionOptions)
{
	if(AAbyssPlayerController* AbyssPC = GetAbyssPlayerControllerFromActorInfo())
	{
		if(UAbyssIndicatorManagerComponent* IndicatorManagerComp = UAbyssIndicatorManagerComponent::GetComponent(AbyssPC))
		{
			for(UIndicatorDescriptor* Indicator : Indicators)
			{
				IndicatorManagerComp->RemoveIndicator(Indicator);
			}
			Indicators.Reset();

			for(const FInteractionOption& Option : InteractionOptions)
			{
				AActor* InteractableActor = UInteractionLibrary::GetActorFromInteractableTarget(Option.InteractableTarget);
				ensure(InteractableActor);
				
				TSoftClassPtr<UUserWidget> InteractionWidgetClass =
					Option.InteractionWidgetClass.IsNull() ? DefaultInteractionWidgetClass : Option.InteractionWidgetClass;

				//为可交互对象分配UI指示器
				UIndicatorDescriptor* Indicator = NewObject<UIndicatorDescriptor>();
				Indicator->Initialize(InteractableActor, InteractableActor->GetRootComponent(), InteractionWidgetClass);
				IndicatorManagerComp->AddIndicator(Indicator);

				Indicators.Add(Indicator);
			}
		}
	}

	CurrentOptions = InteractionOptions;
}

void UAbyssGameplayAbility_Interact::TriggerInteraction()
{
	if(CurrentOptions.IsEmpty())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if(ASC)
	{
		const FInteractionOption& Option = CurrentOptions[0];

		AActor* Instigator = GetAvatarActorFromActorInfo();
		AActor* InteractableActor = UInteractionLibrary::GetActorFromInteractableTarget(Option.InteractableTarget);

		//与交互目标进行交互时，允许目标对象自定义即将传递的事件数据
		//在某些情况下，交互目标可能需要传递特定的信息给互动的能力（例如，额外的数据或状态），而这些信息是只有目标对象自己才知道的
		FGameplayEventData Payload;
		Payload.EventTag = TAG_Ability_Interaction_Activate;
		Payload.Instigator = Instigator;
		Payload.Target = InteractableActor;

		//假设玩家与墙上的一个按钮交互，而这个按钮实际上应该控制附近的门。
		//在这种情况下，按钮可能会将 Target（目标）重定向为门的演员对象，这样玩家的交互实际上会影响门，而不是按钮本身。
		//CustomizeInteractionEventData允许交互目标根据特定的需求调整交互的目标。
		Option.InteractableTarget->CustomizeInteractionEventData(TAG_Ability_Interaction_Activate, Payload);

		//Payload.Target 将作为这次交互的“avatar”（代表玩家交互的实体）。
		//而源自交互目标的演员对象 (InteractableTargetActor) 将作为该交互的“owner”（所有者）。
		//这个区分确保交互过程中“执行者”和“目标”的角色明确分配。
		AActor* TargetActor = const_cast<AActor*>(ToRawPtr(Payload.Target));
		
		FGameplayAbilityActorInfo ActorInfo;
		ActorInfo.InitFromActor(InteractableActor, TargetActor, Option.TargetASC);

		//将通过事件标记（TAG_Ability_Interaction_Activate）来触发互动能力。
		//在这个步骤中，能力系统通过传递事件数据和初始化的演员信息来激活特定的能力，从而响应玩家的交互行为。
		//如果交互成功，交互目标将执行与其关联的游戏能力。
		const bool bSuccess = Option.TargetASC->TriggerAbilityFromGameplayEvent(
			Option.TargetInteractionAbilitySpecHandle,
			&ActorInfo,
			TAG_Ability_Interaction_Activate,
			&Payload,
			*Option.TargetASC);
	}
}



