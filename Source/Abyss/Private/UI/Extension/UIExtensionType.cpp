#include "UI/Extension/UIExtensionType.h"
#include "UI/Extension/UIExtensionSubsystem.h"

void FUIExtensionHandle::Unregister()
{
	if (UUIExtensionSubsystem* Subsystem = ExtensionSource.Get())
	{
		Subsystem->UnregisterExtension(*this);
	}
}

void FUIExtensionPointHandle::Unregister()
{
	if (UUIExtensionSubsystem* Subsystem = ExtensionSource.Get())
	{
		Subsystem->UnregisterExtensionPoint(*this);
	}
}
