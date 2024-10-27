#pragma once

#include "PickupInstance.generated.h"

class UAbyssInventoryItemInstance;
class UAbyssInventoryItemDefinition;

USTRUCT(BlueprintType)
struct FPickupTemplate
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	int32 StackCount = 1;


	UPROPERTY(EditAnywhere)
	TSubclassOf<UAbyssInventoryItemDefinition> ItemDef;
};

USTRUCT(BlueprintType)
struct FPickupInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAbyssInventoryItemInstance> Item = nullptr;
};

USTRUCT(BlueprintType)
struct FInventoryPickup
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPickupInstance> Instances;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPickupTemplate> Templates;
};
