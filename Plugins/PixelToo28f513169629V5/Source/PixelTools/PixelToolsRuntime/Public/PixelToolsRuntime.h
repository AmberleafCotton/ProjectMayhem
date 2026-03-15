//copyright Michael Bochkarev (Turbomorsh)

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FPixelToolsRuntimeModule : public IModuleInterface

{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};