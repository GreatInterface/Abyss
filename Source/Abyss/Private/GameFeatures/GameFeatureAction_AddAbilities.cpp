// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatures/GameFeatureAction_AddAbilities.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AbyssAbilitySet.h"
#include "AbilitySystem/System/AbyssAbilitySystemComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Player/AbyssPlayerState.h"

void UGameFeatureAction_AddAbilities::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);

	if (!ensureAlways(ActiveData.ActiveExtensions.IsEmpty()) ||
		!ensureAlways(ActiveData.ComponentRequests.IsEmpty()))
	{
		Reset(ActiveData);
	}
	
	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddAbilities::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
	FPerContextData* ActiveData = ContextData.Find(Context);

	if (ensure(ActiveData))
	{
		Reset(*ActiveData);
	}
}

void UGameFeatureAction_AddAbilities::AddToWorld(const FWorldContext& WorldContext,
	const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;
	check(World && GameInstance);
	
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if (GameInstance && World && World->IsGameWorld())
	{
		if (UGameFrameworkComponentManager* CompManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			int32 EntryIndex = 0;
			for (const FGameFeatureAbilityEntry& Entry : AbilityList)
			{
				if (!Entry.ActorClass.IsNull())
				{
					auto AddAbilityDelegate =
						UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(
							this, &ThisClass::HandleActorExtension, EntryIndex, ChangeContext);
					TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = CompManager->AddExtensionHandler(Entry.ActorClass, AddAbilityDelegate);

					ActiveData.ComponentRequests.Add(ExtensionRequestHandle);
					++EntryIndex;
				}
			}
		}
	}
}

void UGameFeatureAction_AddAbilities::Reset(FPerContextData& ActiveData)
{
	while (!ActiveData.ActiveExtensions.IsEmpty())
	{
		auto ExtensionIt = ActiveData.ActiveExtensions.CreateIterator();
		
		/** Remove First It else */
		RemoveActorAbilities(ExtensionIt->Key, ActiveData);
	}

	ActiveData.ComponentRequests.Empty();
}

void UGameFeatureAction_AddAbilities::HandleActorExtension(AActor* Actor, FName EventName, int32 EntryIndex,
	FGameFeatureStateChangeContext ChangeContext)
{
	FPerContextData* ActiveData = ContextData.Find(ChangeContext);
	if (AbilityList.IsValidIndex(EntryIndex) && ActiveData)
	{
		const FGameFeatureAbilityEntry& Entry = AbilityList[ EntryIndex ];
		if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
		{
			RemoveActorAbilities(Actor, *ActiveData);
		}
		else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == AAbyssPlayerState::NAME_AbyssAbilityReady))
		{
			AddActorAbilities(Actor, Entry, *ActiveData);
		}
	}
}

void UGameFeatureAction_AddAbilities::AddActorAbilities(AActor* Actor, const FGameFeatureAbilityEntry& AbilityEntry,
	FPerContextData& ActiveData)
{
	check(Actor);
	if (!Actor->HasAuthority() ||
		 ActiveData.ActiveExtensions.Find(Actor) != nullptr)
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = FindOrAddComponentForActor<UAbilitySystemComponent>(Actor, AbilityEntry, ActiveData))
	{
		FActorExtensions AddedExtensions;
		AddedExtensions.Abilities.Reserve		 (AbilityEntry.GrantedAbilities.Num());
		AddedExtensions.Attributes.Reserve		 (AbilityEntry.GrantedAttributes.Num());
		AddedExtensions.AbilitySetHandles.Reserve(AbilityEntry.GrantedAbilitySets.Num());

		for (const FAbyssAbilityGrant& Ability : AbilityEntry.GrantedAbilities)
		{
			if (!Ability.AbilityType.IsNull())
			{
				FGameplayAbilitySpec NewAbilitySpec(Ability.AbilityType.LoadSynchronous());
				FGameplayAbilitySpecHandle AbilityHandle = ASC->GiveAbility(NewAbilitySpec);

				AddedExtensions.Abilities.Add(AbilityHandle);
			}
		}

		for (const FAbyssAttributeSetGrant& Attribute : AbilityEntry.GrantedAttributes)
		{
			TSubclassOf<UAttributeSet> SetType = Attribute.AttributeSetType.LoadSynchronous();
			if (SetType)
			{
				UAttributeSet* NewSet = NewObject<UAttributeSet>(ASC->GetOwner(), SetType);
				if (!Attribute.InitializationData.IsNull())
				{
					UDataTable* InitData = Attribute.InitializationData.LoadSynchronous();
					if(InitData)
					{
						NewSet->InitFromMetaDataTable(InitData);
					}
				}

				AddedExtensions.Attributes.Add(NewSet);
				ASC->AddAttributeSetSubobject(NewSet);
			}
		}

		UAbyssAbilitySystemComponent* AbyssASC = CastChecked<UAbyssAbilitySystemComponent>(ASC);
		for (const TSoftObjectPtr<const UAbyssAbilitySet>& SetPtr : AbilityEntry.GrantedAbilitySets)
		{
			if (const UAbyssAbilitySet* Set = SetPtr.Get())
			{
				Set->GiveToAbilitySystem(AbyssASC, &AddedExtensions.AbilitySetHandles.AddDefaulted_GetRef());
			}
		}

		ActiveData.ActiveExtensions.Add(Actor, AddedExtensions);
	}
}

void UGameFeatureAction_AddAbilities::RemoveActorAbilities(AActor* Actor, FPerContextData& ActiveData)
{
	if (FActorExtensions* ActorExtensions = ActiveData.ActiveExtensions.Find(Actor))
	{
		if (UAbilitySystemComponent* ASC = Actor->FindComponentByClass<UAbilitySystemComponent>())
		{
			for (UAttributeSet* Attribute : ActorExtensions->Attributes)
			{
				ASC->RemoveSpawnedAttribute(Attribute);
			}

			for (FGameplayAbilitySpecHandle Handle : ActorExtensions->Abilities)
			{
				ASC->SetRemoveAbilityOnEnd(Handle);
			}

			UAbyssAbilitySystemComponent* AbyssASC = CastChecked<UAbyssAbilitySystemComponent>(ASC);
			for (auto& SetHandle : ActorExtensions->AbilitySetHandles)
			{
				SetHandle.TakeFromAbilitySystem(AbyssASC);
			}
		}

		ActiveData.ActiveExtensions.Remove(Actor);
	}
}

UActorComponent* UGameFeatureAction_AddAbilities::FindOrAddComponentForActor(UClass* ComponentType, AActor* Actor,
	const FGameFeatureAbilityEntry& Entry, FPerContextData& ActiveData)
{
	UActorComponent* ActorComp = Actor->FindComponentByClass(ComponentType);

	bool bMakeCompRequest = (ActorComp == nullptr);
	if (ActorComp)
	{
		if (ActorComp->CreationMethod == EComponentCreationMethod::Native)
		{
			UObject* CompArchetype = ActorComp->GetArchetype();
			bMakeCompRequest = CompArchetype->HasAnyFlags(RF_ClassDefaultObject);
		}
	}

	if (bMakeCompRequest)
	{
		UWorld* World = GetWorld();
		UGameInstance* GameInstance = World->GetGameInstance();

		if (UGameFrameworkComponentManager* CompManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			TSharedPtr<FComponentRequestHandle> RequestHandle =	CompManager->AddComponentRequest(Entry.ActorClass, ComponentType);
			ActiveData.ComponentRequests.Add(RequestHandle);
		}

		if (!ActorComp)
		{
			ActorComp = Actor->FindComponentByClass(ComponentType);
			ensureAlways(ActorComp);
		}
	}

	return ActorComp;
}


