#include "UI/Extension/UIExtensionType.h"
#include "UI/Extension/UIExtensionSubsystem.h"

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
