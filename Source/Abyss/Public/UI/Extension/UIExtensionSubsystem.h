#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UIExtensionType.h"
#include "UIExtensionSubsystem.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FExtendExtensionPointDynamicDelegate, EUIExtensionAction, Action, const FUIExtensionRequest&, ExtensionRequest);

UCLASS()
class ABYSS_API UUIExtensionSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	FUIExtensionPointHandle RegisterExtensionPoint(const FGameplayTag& PointTag, EUIExtensionPointMatch ExtensionPointMatch, const TArray<UClass*>& AllowedDataClasses, FExtendExtensionPointDelegate ExtensionCallback);
	FUIExtensionPointHandle RegisterExtensionPointForContext(UObject* ContextObject, const FGameplayTag& PointTag, EUIExtensionPointMatch ExtensionPointMatch, const TArray<UClass*>& AllowedDataClasses, FExtendExtensionPointDelegate ExtensionCallback);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
	void UnregisterExtension(const FUIExtensionHandle& ExtensionHandle);
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
	void UnregisterExtensionPoint(const FUIExtensionPointHandle& ExtensionPointHandle);
	
protected:
	//~Subsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~End of Subsystem

	FUIExtensionRequest CreateExtensionRequest(const TSharedPtr<FUIExtension>& Extension);

	void NotifyExtensionPointOfExtensions(TSharedPtr<FUIExtensionPoint>& ExtensionPoint);
	void NotifyExtensionPointsOfExtension(EUIExtensionAction Action, TSharedPtr<FUIExtension>& Extension);
	
private:
	using FExtensionPointList = TArray<TSharedPtr<FUIExtensionPoint>>;
	TMap<FGameplayTag, FExtensionPointList> ExtensionPointMap;

	using FExtensionList = TArray<TSharedPtr<FUIExtension>>;
	TMap<FGameplayTag, FExtensionList> ExtensionMap;
};
