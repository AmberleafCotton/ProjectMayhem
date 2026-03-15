//copyright Michael Bochkarev (Turbomorsh)
//Based on Mateo Egey code
//(https://herr-edgy.com/tutorials/extending-tool-menus-in-the-editor-via-c/?unapproved=677&moderation-hash=50b15b13ba25e82cbb2dc86d6d41919d#comment-677)

#include "PixelateTextureStyle.h"

#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleMacros.h"

TSharedPtr<FPixelateTextureStyle> FPixelateTextureStyle::MageHubStyle = nullptr;

void FPixelateTextureStyle::Register()
{
	FSlateStyleRegistry::RegisterSlateStyle(Get());
}

void FPixelateTextureStyle::Unregister()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(Get());
}

void FPixelateTextureStyle::Shutdown()
{
	Unregister();
	MageHubStyle.Reset();
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon32x32(32.0f, 32.0f);
const FVector2D Icon64x64(64.0f, 64.0f);
const FVector2D Icon128x128(128.0f, 128.0f);

FPixelateTextureStyle::FPixelateTextureStyle() : FSlateStyleSet(TEXT("PixelTools"))
{
	if(FPaths::DirectoryExists(FPaths::ProjectPluginsDir() / TEXT("PixelTools/Content/Slate")))
	{
		SetContentRoot(FPaths::ProjectPluginsDir() / TEXT("PixelTools/Content/Slate"));
	}
	else
	{
		SetContentRoot(FPaths::EnginePluginsDir() / TEXT("Marketplace/PixelTools/Content/Slate"));
	}
	InitIcons();
}

void FPixelateTextureStyle::InitIcons()
{
	// ToolMenus Icon
	Set("PixelateTexture", new IMAGE_BRUSH("PixelTools_IconDefault_Big", Icon32x32));
	Set("PixelateTextureSmaller", new IMAGE_BRUSH("PixelTools_IconDefault_Small", Icon16x16));
	// Actors Icon
	Set("ClassIcon.PixelPerfectCamera", new IMAGE_BRUSH("PixelToolsActorIcon_Small", Icon16x16));
	Set("ClassThumbnail.PixelPerfectCamera", new IMAGE_BRUSH("PixelToolsActorIcon", Icon64x64));
	// Components Icon
	Set("ClassIcon.PixelPerfectCameraComponent", new IMAGE_BRUSH("PixelToolsComponentIcon", Icon16x16));
	Set("ClassThumbnail.PixelPerfectCameraComponent", new IMAGE_BRUSH("PixelToolsComponentIcon", Icon64x64));

	Set("ClassIcon.PixelPerfectGameInstance", new IMAGE_BRUSH("PixelTools_IconDefault_Small", Icon16x16));
	Set("ClassThumbnail.PixelPerfectGameInstance", new IMAGE_BRUSH("PixelTools_IconDefault_Big", Icon64x64));

	Set("ClassIcon.PixelPerfectViewportClient", new IMAGE_BRUSH("PixelTools_GameViewportClient_Small", Icon16x16));
	Set("ClassThumbnail.PixelPerfectViewportClient", new IMAGE_BRUSH("PixelTools_GameViewportClient_BIG", Icon64x64));

	Set("ClassIcon.PixelPerfectGameUserSettings", new IMAGE_BRUSH("PixelTools_GameUserSettings_Small", Icon16x16));
	Set("ClassThumbnail.PixelPerfectGameUserSettings", new IMAGE_BRUSH("PixelTools_GameUserSettings_Big", Icon64x64));
}

void FPixelateTextureStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const FPixelateTextureStyle& FPixelateTextureStyle::Get()
{
	if(!MageHubStyle.IsValid())
	{
		MageHubStyle = MakeShareable(new FPixelateTextureStyle());
	}

	return *MageHubStyle;
}

void FPixelateTextureStyle::ReinitializeStyle()
{
	Unregister();
	MageHubStyle.Reset();
	Register();	
}