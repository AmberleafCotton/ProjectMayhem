//copyright Michael Bochkarev (Turbomorsh)
//Based on Mateo Egey code
//(https://herr-edgy.com/tutorials/extending-tool-menus-in-the-editor-via-c/?unapproved=677&moderation-hash=50b15b13ba25e82cbb2dc86d6d41919d#comment-677)

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FPixelateTextureStyle : public FSlateStyleSet
{
public:
	static void Register();
	static void Unregister();
	static void Shutdown();

	// reloads textures used by slate renderer
	static void ReloadTextures();

	// return The Slate style set
	static const FPixelateTextureStyle& Get();

	static void ReinitializeStyle();

private:
	FPixelateTextureStyle();
	
	void InitIcons();
	
	static TSharedPtr<FPixelateTextureStyle> MageHubStyle;
};
