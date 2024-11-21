#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "../InteractableTarget.h"
#include "AbyssInteractMessageType.generated.h"


UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_INTENACTION_DURATION_MESSAGE)

UENUM(BlueprintType)
enum class EAbyssInteractionState : uint8
{
	None,

	Equip
};

USTRUCT(BlueprintType)
struct FAbyssInteractDurationMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Instigator = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float Duration = 0;
};

USTRUCT(BlueprintType)
struct FAbyssInteractMessage
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Instigator = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	TArray<TScriptInterface<IInteractableTarget>> Targets;
};

USTRUCT(BlueprintType)
struct FAbyssEquippedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Equipment = nullptr;

	UPROPERTY(BlueprintReadWrite)
	EAbyssInteractionState State = EAbyssInteractionState::None;

	UPROPERTY(BlueprintReadWrite)
	bool bSuccessEquipped = false;
};
