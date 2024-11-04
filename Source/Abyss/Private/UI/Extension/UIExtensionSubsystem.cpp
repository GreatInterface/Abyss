// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Extension/UIExtensionSubsystem.h"
#include "Blueprint/UserWidget.h"


DEFINE_LOG_CATEGORY_STATIC(LogUIExtension, Log, All);

void FUIExtensionHandle::Unregister()
{
	if (UUIExtensionSubsystem* Subsystem = ExtensionSource.Get())
	{
		Subsystem->UnregisterExtension(*this);
	}
}

bool FUIExtensionPoint::DoesExtensionPassContract(const FUIExtension* Extension) const
{
	if (UObject* DataPtr = Extension->Data)
	{
		const bool bMatchesContext =
			(ContextObject.IsExplicitlyNull() && Extension->ContextObject.IsExplicitlyNull()) ||
			(ContextObject == Extension->ContextObject);

		if (!bMatchesContext)
			return false;

		const UClass* DataClass = DataPtr->IsA(UClass::StaticClass()) ? Cast<UClass>(DataPtr) : DataPtr->GetClass();
		for (const UClass* AllowDataClass : AllowedDataClasses)
		{
			if (DataClass->IsChildOf(AllowDataClass) || DataClass->ImplementsInterface(AllowDataClass))
			{
				return true;
			}
		}
	}

	return false;
}

void FUIExtensionPointHandle::Unregister()
{
	if (UUIExtensionSubsystem* Subsystem = ExtensionSource.Get())
	{
		Subsystem->UnregisterExtensionPoint(*this);
	}
}

//----------------------------------------------------------------------------------------------------------------------
//	UUIExtensionSubsystem
//----------------------------------------------------------------------------------------------------------------------

FUIExtensionPointHandle UUIExtensionSubsystem::RegisterExtensionPoint(const FGameplayTag& PointTag,
	EUIExtensionPointMatch ExtensionPointMatch, const TArray<UClass*>& AllowedDataClasses,
	FExtendExtensionPointDelegate ExtensionCallback)
{
	return RegisterExtensionPointForContext(nullptr, PointTag, ExtensionPointMatch, AllowedDataClasses, ExtensionCallback); 	
}

FUIExtensionPointHandle UUIExtensionSubsystem::RegisterExtensionPointForContext(UObject* ContextObject,
	const FGameplayTag& PointTag, EUIExtensionPointMatch ExtensionPointMatch, const TArray<UClass*>& AllowedDataClasses,
	FExtendExtensionPointDelegate ExtensionCallback)
{
	if (!PointTag.IsValid())
	{
		UE_LOG(LogUIExtension, Warning, TEXT("Trying to register an invalid extension point."));
		return FUIExtensionPointHandle();
	}

	if (!ExtensionCallback.IsBound())
	{
		UE_LOG(LogUIExtension, Warning, TEXT("Trying to register an invalid extension point"));
		return FUIExtensionPointHandle();
	}

	if (AllowedDataClasses.IsEmpty())
	{
		UE_LOG(LogUIExtension, Warning, TEXT("Trying to register an invalid extension point"));
		return FUIExtensionPointHandle();
	}

	FExtensionPointList& List = ExtensionPointMap.FindOrAdd(PointTag);

	TSharedPtr<FUIExtensionPoint>& Entry = List.Add_GetRef(MakeShared<FUIExtensionPoint>());
	Entry->ExtensionPointTag = PointTag;
	Entry->ContextObject = ContextObject;
	Entry->AllowedDataClasses = AllowedDataClasses;
	Entry->ExtensionPointTagMatchType = ExtensionPointMatch;
	Entry->Callback = MoveTemp(ExtensionCallback);

	UE_LOG(LogUIExtension, Verbose, TEXT("Extension Point [%s] Registered"), *PointTag.ToString());

	NotifyExtensionPointOfExtensions(Entry);

	return FUIExtensionPointHandle(this, Entry);
}

FUIExtensionHandle UUIExtensionSubsystem::RegisterExtensionAsData(UObject* ContextObject, const FGameplayTag& PointTag,
	UObject* Data, int32 Priority)
{
	if (!PointTag.IsValid())
	{
		UE_LOG(LogUIExtension, Warning, TEXT("Trying to register an invalid extension."));
		return FUIExtensionHandle(); 
	}

	if (!Data)
	{
		UE_LOG(LogUIExtension, Warning, TEXT("Trying to register an invalid extension."));
		return FUIExtensionHandle();
	}

	FExtensionList& List = ExtensionMap.FindOrAdd(PointTag);

	TSharedPtr<FUIExtension> Extension = List.Add_GetRef(MakeShared<FUIExtension>());
	Extension->Data = Data;
	Extension->Priority = Priority;
	Extension->ContextObject = ContextObject;
	Extension->ExtensionPointTag = PointTag;

	if (ContextObject)
	{
		UE_LOG(LogUIExtension, Verbose, TEXT("Extension [%s] @ [%s] Registered"), *GetNameSafe(Data), *PointTag.ToString());
	}
	else
	{
		UE_LOG(LogUIExtension, Verbose, TEXT("Extension [%s] for [%s] @ [%s] Registered"), *GetNameSafe(Data), *GetNameSafe(ContextObject), *PointTag.ToString());
	}

	NotifyExtensionPointsOfExtension(EUIExtensionAction::Added, Extension);

	return FUIExtensionHandle(this, Extension);
}

FUIExtensionHandle UUIExtensionSubsystem::RegisterExtensionAsWidget(const FGameplayTag& PointTag,
	TSubclassOf<UUserWidget> WidgetClass, int32 Priority)
{
	return RegisterExtensionAsData(nullptr, PointTag, WidgetClass, Priority);
}

FUIExtensionHandle UUIExtensionSubsystem::RegisterExtensionAsWidgetForContext(UObject* ContextObject,
                                                                              const FGameplayTag& PointTag, TSubclassOf<UUserWidget> WidgetClass, int32 Priority)
{
	return RegisterExtensionAsData(ContextObject, PointTag, WidgetClass, Priority);
}

void UUIExtensionSubsystem::UnregisterExtension(const FUIExtensionHandle& ExtensionHandle)
{
	if (!ExtensionHandle.IsValid())
	{
		UE_LOG(LogUIExtension, Warning, TEXT("Trying to unregister an invalid Handle."));
		return;
	}

	checkf(ExtensionHandle.ExtensionSource == this, TEXT("Trying to Unregister an extension that's not from this extension subsystem"));

	TSharedPtr<FUIExtension> Extension = ExtensionHandle.DataPtr;
	if (FExtensionList* List = ExtensionMap.Find(Extension->ExtensionPointTag))
	{
		if (Extension->ContextObject.IsExplicitlyNull())
		{
			UE_LOG(LogUIExtension, Verbose, TEXT("Extension [%s] @ [%s] Unregistered"), *GetNameSafe(Extension->Data), *Extension->ExtensionPointTag.ToString());
		}
		else
		{
			UE_LOG(LogUIExtension, Verbose, TEXT("Extension [%s] for [%s] @ [%s] Unregistered"), *GetNameSafe(Extension->Data), *GetNameSafe(Extension->ContextObject.Get()), *Extension->ExtensionPointTag.ToString());
		}

		NotifyExtensionPointsOfExtension(EUIExtensionAction::Removed, Extension);

		List->RemoveSwap(Extension);

		if (List->IsEmpty())
		{
			ExtensionMap.Remove(Extension->ExtensionPointTag);
		}
	}
}

void UUIExtensionSubsystem::UnregisterExtensionPoint(const FUIExtensionPointHandle& ExtensionPointHandle)
{
	if (!ExtensionPointHandle.IsValid())
	{
		UE_LOG(LogUIExtension, Warning, TEXT("Trying to unregister an invalid Handle."));
		return;
	}

	checkf(ExtensionPointHandle.ExtensionSource == this, TEXT("Trying to Unregister an extension that's not from this extension subsystem"));

	TSharedPtr<FUIExtensionPoint> Point = ExtensionPointHandle.DataPtr;
	if (FExtensionPointList* List = ExtensionPointMap.Find(Point->ExtensionPointTag))
	{
		UE_LOG(LogUIExtension, Verbose, TEXT("Extension Point [%s] Unregistered"), *Point->ExtensionPointTag.ToString());

		List->RemoveSwap(Point);
		if (List->IsEmpty())
		{
			ExtensionPointMap.Remove(Point->ExtensionPointTag);
		}
	}
}

void UUIExtensionSubsystem::AddReferencedObjects(UObject* InThis, FReferenceCollector& Controller)
{
	if (UUIExtensionSubsystem* Subsystem = Cast<UUIExtensionSubsystem>(InThis))
	{
		auto PointIt = Subsystem->ExtensionPointMap.CreateIterator();
		for (; PointIt; ++PointIt)
		{
			for (const TSharedPtr<FUIExtensionPoint>& Value : PointIt.Value())
			{
				Controller.AddReferencedObjects(Value->AllowedDataClasses);
			}
		}

		auto ExtensionIt = Subsystem->ExtensionMap.CreateIterator();
		for(; ExtensionIt; ++ExtensionIt)
		{
			for (const TSharedPtr<FUIExtension>& Value : ExtensionIt.Value())
			{
				Controller.AddReferencedObject(Value->Data);
			}
		}
	}
}

void UUIExtensionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UUIExtensionSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

FUIExtensionRequest UUIExtensionSubsystem::CreateExtensionRequest(const TSharedPtr<FUIExtension>& Extension)
{
	FUIExtensionRequest Request;
	Request.Data = Extension->Data;
	Request.Priority = Extension->Priority;
	Request.ContextObject = Extension->ContextObject.Get();
	Request.ExtensionHandle = FUIExtensionHandle(this, Extension);
	Request.ExtensionPointTag = Extension->ExtensionPointTag;

	return Request;
}

void UUIExtensionSubsystem::NotifyExtensionPointOfExtensions(TSharedPtr<FUIExtensionPoint>& ExtensionPoint)
{
	for (FGameplayTag Tag = ExtensionPoint->ExtensionPointTag; Tag.IsValid(); Tag = Tag.RequestDirectParent())
	{
		if (const FExtensionList* List = ExtensionMap.Find(Tag))
		{
			FExtensionList CopyList(*List);

			for (const TSharedPtr<FUIExtension>& Extension : CopyList)
			{
				if (ExtensionPoint->DoesExtensionPassContract(Extension.Get()))
				{
					FUIExtensionRequest Request = CreateExtensionRequest(Extension);
					ExtensionPoint->Callback.ExecuteIfBound(EUIExtensionAction::Added, Request);
				}
			}
		}

		if (ExtensionPoint->ExtensionPointTagMatchType == EUIExtensionPointMatch::ExactMatch)
		{
			break;
		}
	}
}

void UUIExtensionSubsystem::NotifyExtensionPointsOfExtension(EUIExtensionAction Action,
	TSharedPtr<FUIExtension>& Extension)
{
	for (FGameplayTag Tag = Extension->ExtensionPointTag; Tag.IsValid(); Tag = Tag.RequestDirectParent())
	{
		if (const FExtensionPointList* List = ExtensionPointMap.Find(Tag))
		{
			FExtensionPointList CopyList(*List);

			for (const TSharedPtr<FUIExtensionPoint>& ExtensionPoint : CopyList)
			{
				if (ExtensionPoint->DoesExtensionPassContract(Extension.Get()))
				{
					FUIExtensionRequest Request = CreateExtensionRequest(Extension);
					ExtensionPoint->Callback.ExecuteIfBound(Action, Request);
				}
			}
		}
	}
}

FUIExtensionHandle UUIExtensionSubsystem::K2_RegisterExtensionAsWidgetForContext(FGameplayTag ExtensionPointTag,
	TSubclassOf<UUserWidget> WidgetClass, UObject* ContextObject, int32 Priority)
{
	if (ContextObject)
	{
		return RegisterExtensionAsWidgetForContext(ContextObject, ExtensionPointTag, WidgetClass, Priority);
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("A null ContextObject was passed to Register Extension (Widget For Context)"), ELogVerbosity::Error);
		return FUIExtensionHandle();
	}
}

FUIExtensionHandle UUIExtensionSubsystem::K2_RegisterExtensionAsData(FGameplayTag ExtensionPointTag, UObject* Data,
	int32 Priority)
{
	return RegisterExtensionAsData(nullptr, ExtensionPointTag, Data, Priority);
}
