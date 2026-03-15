//copyright Michael Bochkarev (Turbomorsh)

#include "PixelToolsRuntime.h"

#define LOCTEXT_NAMESPACE "FPixelToolsRuntimeModule"

//Register module
void FPixelToolsRuntimeModule::StartupModule()
{
	IModuleInterface::StartupModule();
}

void FPixelToolsRuntimeModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
	// This function may be called during shutdown to clean up your module. For modules that support dynamic reloading,
	// we call this function before unloading the module
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPixelToolsRuntimeModule, PixelToolsRuntime)