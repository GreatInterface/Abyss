#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "UIExtensionSubsystem.generated.h"

UENUM(BlueprintType)
enum class EUIExtensionPointMatch : uint8
{
	ExactMatch,

	PartialMatch
};

UENUM(BlueprintType)
enum class EUIExtensionAction : uint8
{
	Added,
	Removed
};

/**
 * FUIExtension
 *
 * Represents a UI element or data that can be registered to an extension point.
 * An extension is typically a widget or UI component that fulfills a specific 
 * purpose and is associated with a particular gameplay tag. Each extension has a 
 * priority level, allowing it to be organized within an extension point, and can 
 * optionally be context-specific.
 *
 * Usage:
 * - Register an extension at a designated extension point to add custom UI elements.
 * - Extensions can be prioritized to define their display order.
 * - Use context objects to bind extensions to specific gameplay or UI contexts.
 */
struct FUIExtension : TSharedFromThis<FUIExtension>
{
public:
	int32 Priority = INDEX_NONE;
	
	TObjectPtr<UObject> Data = nullptr;
	
	TWeakObjectPtr<UObject> ContextObject;
	
	FGameplayTag ExtensionPointTag;
};


USTRUCT(BlueprintType)
struct ABYSS_API FUIExtensionHandle
{
	GENERATED_BODY()

public:
	FUIExtensionHandle() {}

	void Unregister();

	bool IsValid() const { return DataPtr.IsValid(); }

	bool operator==(const FUIExtensionHandle& Other) const { return DataPtr == Other.DataPtr; }
	bool operator!=(const FUIExtensionHandle& Other) const { return !operator==(Other); }

	friend FORCEINLINE uint32 GetTypeHash(FUIExtensionHandle Handle)
	{
		return PointerHash(Handle.DataPtr.Get());
	}
	
private:

	FUIExtensionHandle(UUIExtensionSubsystem* InSubsystem, const TSharedPtr<FUIExtension>& InDataPtr)
		: ExtensionSource(InSubsystem)
		, DataPtr(InDataPtr)
	{}

private:
	
	TWeakObjectPtr<UUIExtensionSubsystem> ExtensionSource;

	TSharedPtr<FUIExtension> DataPtr;

	friend UUIExtensionSubsystem;
};

template<>
struct TStructOpsTypeTraits<FUIExtensionHandle> : public TStructOpsTypeTraitsBase2<FUIExtensionHandle>
{
	enum
	{
		WithCopy = true,  // This ensures the opaque type is copied correctly in BPs
		WithIdenticalViaEquality = true,
	};
};

//----------------------------------------------------------------------------------------------------------------------
//		ExtensionPoint
//----------------------------------------------------------------------------------------------------------------------

DECLARE_DELEGATE_TwoParams(FExtendExtensionPointDelegate, EUIExtensionAction Action, const FUIExtensionRequest& Request);

/**
 * FUIExtensionPoint
 *
 * Represents a "point of extension" in the UI system, where different UI elements
 * can be dynamically added or removed at runtime. Extension points are designated by a 
 * gameplay tag and specify a context and allowed data types, defining the type of extensions 
 * that can register themselves to this point. This enables flexibility in UI composition 
 * by allowing various modules to add their custom UI components to pre-defined regions.
 *
 * Usage:
 * - Register an extension point to define a customizable section in the UI.
 * - Register only specific data types by defining allowed data classes.
 * - Supports context-based filtering, where an extension can be bound to a specific 
 *   game context.
 */
struct FUIExtensionPoint : TSharedFromThis<FUIExtensionPoint>
{
	EUIExtensionPointMatch ExtensionPointTagMatchType = EUIExtensionPointMatch::ExactMatch;
	
	TWeakObjectPtr<UObject> ContextObject;

	TArray<TObjectPtr<UClass>> AllowedDataClasses;

	FExtendExtensionPointDelegate Callback;

	FGameplayTag ExtensionPointTag;

	bool DoesExtensionPassContract(const FUIExtension* Extension)const;
};

USTRUCT(BlueprintType)
struct ABYSS_API FUIExtensionPointHandle
{
	GENERATED_BODY()

public:
	FUIExtensionPointHandle(){}

	void Unregister();

	bool IsValid() const { return DataPtr.IsValid(); }

	bool operator==(const FUIExtensionPointHandle& Other) const { return DataPtr == Other.DataPtr; }
	bool operator!=(const FUIExtensionPointHandle& Other) const { return !operator==(Other); }

	friend uint32 GetTypeHash(const FUIExtensionPointHandle& Handle)
	{
		return PointerHash(Handle.DataPtr.Get());
	}
	
private:

	FUIExtensionPointHandle(UUIExtensionSubsystem* InExtensionSystem, const TSharedPtr<FUIExtensionPoint>& InDataPtr)
		: ExtensionSource(InExtensionSystem)
		, DataPtr(InDataPtr)
	{}
private:
	TWeakObjectPtr<UUIExtensionSubsystem> ExtensionSource;

	TSharedPtr<FUIExtensionPoint> DataPtr;

	friend UUIExtensionSubsystem;
};

template<>
struct TStructOpsTypeTraits<FUIExtensionPointHandle> : public TStructOpsTypeTraitsBase2<FUIExtensionPointHandle>
{
	enum
	{
		WithCopy = true,
		WithIdenticalViaEquality = true,
	};
};

//----------------------------------------------------------------------------------------------------------------------
//		Request
//----------------------------------------------------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct ABYSS_API FUIExtensionRequest
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FUIExtensionHandle ExtensionHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag ExtensionPointTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Priority = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UObject> Data = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UObject> ContextObject = nullptr;
};


DECLARE_DYNAMIC_DELEGATE_TwoParams(FExtendExtensionPointDynamicDelegate, EUIExtensionAction, Action, const FUIExtensionRequest&, ExtensionRequest);

UCLASS()
class ABYSS_API UUIExtensionSubsystem : public UWorldSubsystem
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
