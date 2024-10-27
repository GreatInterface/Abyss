// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#ifndef ENABLE_DRAW_DEBUG
#define ENABLE_DRAW_DEBUG 1
#endif

class FAbyssCoreRuntimeModule : public IModuleInterface
{
public:
	//~IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~End of IModuleInterface
};
