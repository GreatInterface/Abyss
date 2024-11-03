// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Extension/UIExtensionSubsystem.h"
#include "UI/Extension/UIExtensionType.h"

DEFINE_LOG_CATEGORY_STATIC(LogUIExtension, Log, All);

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

void UUIExtensionSubsystem::UnregisterExtension(const FUIExtensionHandle& ExtensionHandle)
{
}

void UUIExtensionSubsystem::UnregisterExtensionPoint(const FUIExtensionPointHandle& ExtensionPointHandle)
{
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
