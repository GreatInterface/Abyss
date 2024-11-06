#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "AbyssInteractMessageType.generated.h"


UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_INTENACTION_DURATION_MESSAGE)

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