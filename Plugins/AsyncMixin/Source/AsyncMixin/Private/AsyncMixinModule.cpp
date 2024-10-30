#include "Modules/ModuleManager.h"


class FAsyncMixinModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

void FAsyncMixinModule::StartupModule()
{
	IModuleInterface::StartupModule();
}

void FAsyncMixinModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
}

IMPLEMENT_MODULE(FAsyncMixinModule, AsyncMixin)