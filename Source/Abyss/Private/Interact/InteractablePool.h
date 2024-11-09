#pragma once

#include "InteractablePool.generated.h"

class AAbyssInteractable;

USTRUCT()
struct FInteractablePool
{
	GENERATED_BODY()


private:

	UPROPERTY(Transient)
	TArray<TObjectPtr<AAbyssInteractable>> ActiveInteractables;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AAbyssInteractable>> InactiveInteractables;

	TWeakObjectPtr<UWorld> OwningWorld;
	TWeakObjectPtr<APlayerController> DefaultPlayerController;
};
