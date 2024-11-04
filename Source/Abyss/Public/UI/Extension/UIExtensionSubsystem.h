#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UIExtensionType.h"
#include "Subsystems/WorldSubsystem.h"
#include "UIExtensionSubsystem.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FExtendExtensionPointDynamicDelegate, EUIExtensionAction, Action, const FUIExtensionRequest&, ExtensionRequest);

UCLASS()
class UUIExtensionSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	FUIExtensionPointHandle RegisterExtensionPoint(const FGameplayTag& PointTag, EUIExtensionPointMatch ExtensionPointMatch, const TArray<UClass*>& AllowedDataClasses, FExtendExtensionPointDelegate ExtensionCallback);
	FUIExtensionPointHandle RegisterExtensionPointForContext(UObject* ContextObject, const FGameplayTag& PointTag, EUIExtensionPointMatch ExtensionPointMatch, const TArray<UClass*>& AllowedDataClasses, FExtendExtensionPointDelegate ExtensionCallback);

	FUIExtensionHandle RegisterExtensionAsData(UObject* ContextObject, const FGameplayTag& PointTag, UObject* Data, int32 Priority);
	FUIExtensionHandle RegisterExtensionAsWidget(const FGameplayTag& PointTag, TSubclassOf<UUserWidget> WidgetClass, int32 Priority);
	FUIExtensionHandle RegisterExtensionAsWidgetForContext(UObject* ContextObject, const FGameplayTag& PointTag, TSubclassOf<UUserWidget> WidgetClass, int32 Priority);
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
	void UnregisterExtension(const FUIExtensionHandle& ExtensionHandle);
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
	void UnregisterExtensionPoint(const FUIExtensionPointHandle& ExtensionPointHandle);

	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Controller);
	
protected:
	//~Subsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~End of Subsystem

	FUIExtensionRequest CreateExtensionRequest(const TSharedPtr<FUIExtension>& Extension);

	void NotifyExtensionPointOfExtensions(TSharedPtr<FUIExtensionPoint>& ExtensionPoint);
	void NotifyExtensionPointsOfExtension(EUIExtensionAction Action, TSharedPtr<FUIExtension>& Extension);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension", meta = (DisplayName = "Register Extension (Widget For Context)"))
	FUIExtensionHandle K2_RegisterExtensionAsWidgetForContext(FGameplayTag ExtensionPointTag, TSubclassOf<UUserWidget> WidgetClass, UObject* ContextObject, int32 Priority = -1);

	/**
	 * Registers the extension as data for any extension point that can make use of it.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="UI Extension", meta = (DisplayName = "Register Extension (Data)"))
	FUIExtensionHandle K2_RegisterExtensionAsData(FGameplayTag ExtensionPointTag, UObject* Data, int32 Priority = -1);

	
private:
	using FExtensionPointList = TArray<TSharedPtr<FUIExtensionPoint>>;
	TMap<FGameplayTag, FExtensionPointList> ExtensionPointMap;

	using FExtensionList = TArray<TSharedPtr<FUIExtension>>;
	TMap<FGameplayTag, FExtensionList> ExtensionMap;
};
