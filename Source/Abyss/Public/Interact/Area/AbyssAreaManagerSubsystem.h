// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AbyssAreaManagerSubsystem.generated.h"


class AAbyssInteractable_AreaGate;


USTRUCT(BlueprintType)
struct FAbyssAreaHandle
{
	GENERATED_BODY()
	
	friend struct FAbyssAreaHandle;
public:
	FAbyssAreaHandle()
		: ID(INDEX_NONE)
	{}

	void GenerateNewHandle()
	{
		static int32 GHandle = 1;
		ID = GHandle++;
	}

	bool IsValid() const { return ID != INDEX_NONE; }

	bool operator==(const FAbyssAreaHandle& Other) const { return ID == Other.ID; }
	bool operator!=(const FAbyssAreaHandle& Other) const { return !operator==(Other); }

	friend uint32 GetTypeHash(const FAbyssAreaHandle& Handle)
	{
		return ::GetTypeHash(Handle.ID);
	}
private:
	
	UPROPERTY()
	int32 ID = 0;
};



DECLARE_MULTICAST_DELEGATE_OneParam(FOnAreaDelegate, FAbyssAreaHandle)


UCLASS()
class ABYSS_API UAbyssAreaManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:

	//~Subsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	void BeginEntryArea(FAbyssAreaHandle AreaHandle);
	void BeginExitArea(FAbyssAreaHandle AreaHandle);

private:

	void HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName);
	void HandleUnloadMap();
	
	TMap<FAbyssAreaHandle, TObjectPtr<AAbyssInteractable_AreaGate>> AreaGatesMap;

	FOnAreaDelegate OnEnteredArea;
	FOnAreaDelegate OnExitedArea;

	friend class AAbyssInteractable_AreaGate;
};
