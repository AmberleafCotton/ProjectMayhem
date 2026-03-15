//copyright Michael Bochkarev (Turbomorsh)
//Based on Mateo Egey code
//(https://herr-edgy.com/tutorials/extending-tool-menus-in-the-editor-via-c/?unapproved=677&moderation-hash=50b15b13ba25e82cbb2dc86d6d41919d#comment-677)

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FPixelToolsEditorModule : public IModuleInterface

{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void PixelateProject();

private:
	void RegisterMenus();
};
