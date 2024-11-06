#pragma once

#include "GameplayAbilitySpecHandle.h"
#include "Abilities/GameplayAbility.h"
#include "InteractionOption.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
class IInteractableTarget;

USTRUCT(BlueprintType, Blueprintable)
struct FInteractionOption
{
	GENERATED_BODY()
public:

	//玩家可以交互的对象
	UPROPERTY(BlueprintReadWrite)
	TScriptInterface<IInteractableTarget> InteractableTarget;

	//主交互文本
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text;

	//附加描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SubText;

	// 1) 当进行交互时，需要为玩家授予的Ability
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> InteractionAbilityToGrant;

	// 2) 让与我们互动的对象拥有自己的能力系统和互动能力，而不是由我们来激活。
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> TargetASC = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FGameplayAbilitySpecHandle TargetInteractionAbilitySpecHandle;

	/*-----------------------------------------------------------------------------------------
			UI
	 ----------------------------------------------------------------------------------------*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UUserWidget> InteractionWidgetClass;

public:

	bool operator==(const FInteractionOption& Other) const noexcept
	{
		return InteractableTarget == Other.InteractableTarget &&
			InteractionAbilityToGrant == Other.InteractionAbilityToGrant &&
			TargetASC == Other.TargetASC &&
			TargetInteractionAbilitySpecHandle == Other.TargetInteractionAbilitySpecHandle &&
			Text.IdenticalTo(Other.Text) &&
			SubText.IdenticalTo(Other.SubText);
	}

	bool operator!=(const FInteractionOption& Other) const
	{
		return !operator==(Other);
	}

	bool operator<(const FInteractionOption& Other) const
	{
		return InteractableTarget.GetInterface() < Other.InteractableTarget.GetInterface();
	}
};

class FInteractionOptionBuilder
{
public:
	FInteractionOptionBuilder(const TScriptInterface<IInteractableTarget>& InScope, TArray<FInteractionOption>& InteractionOptions)
		: Scope(InScope)
		, Options(InteractionOptions)
	{}

	void AddInteractionOption(const FInteractionOption& InOption)
	{
		FInteractionOption& OptionEntry = Options.Add_GetRef(InOption);
		OptionEntry.InteractableTarget = Scope;
	}
	
private:
	TScriptInterface<IInteractableTarget> Scope;
	TArray<FInteractionOption>& Options;
};
