//copyright Michael Bochkarev (Turbomorsh)
//Based on Mateo Egey code
//(https://herr-edgy.com/tutorials/extending-tool-menus-in-the-editor-via-c/?unapproved=677&moderation-hash=50b15b13ba25e82cbb2dc86d6d41919d#comment-677)

#include "PixelToolsEditor.h"

#include "ContentBrowserMenuContexts.h"
#include "GameMapsSettings.h"
#include "GeneralProjectSettings.h"
#include "IPlacementModeModule.h"
#include "ToolMenus.h"
#include "ToolMenuSection.h"
#include "PixelateTextureStyle.h"
#include "PixelPerfectCamera.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/RendererSettings.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Framework/Notifications/NotificationManager.h"
#include "GameFramework/InputSettings.h"
#include "Utils/PixelPerfectGameUserSettings.h"
#include "Utils/PixelPerfectViewportClient.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "FPixelToolsEditorModule"

//Register module
void FPixelToolsEditorModule::StartupModule()
{
	//Register SlateStyle for custom entry menu buttons
	FPixelateTextureStyle::Register();
	
	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FPixelToolsEditorModule::RegisterMenus));
	
	//Create and register new category in Place Actors menu
	const FPlacementCategoryInfo Info(
	   INVTEXT("Pixelate Capture"),
	   FSlateIcon(FPixelateTextureStyle::Get().GetStyleSetName(), "PixelateTextureSmaller"),
	   "PixelCapture",
	   TEXT("PixelCapture"),
	   4
   );
	
	IPlacementModeModule& PlacementModeModule = IPlacementModeModule::Get();
	PlacementModeModule.RegisterPlacementCategory(Info);
 
	// Add actor classes to the category
	PlacementModeModule.RegisterPlaceableItem(Info.UniqueHandle,
		MakeShared<FPlaceableItem>(nullptr, FAssetData(APixelPerfectCamera::StaticClass())));
	
}

void FPixelToolsEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module. For modules that support dynamic reloading,
	// we call this function before unloading the module
	
	// Unregister the custom category when the module is shut down
	if (IPlacementModeModule::IsAvailable())
	{
		IPlacementModeModule::Get().UnregisterPlacementCategory("PixelCapture");
	}
}

//Add this module to tool menu
void FPixelToolsEditorModule::RegisterMenus()
{
	UToolMenu* TextureMenu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.Texture2D");
	FToolMenuSection& TextureSection = TextureMenu->FindOrAddSection("GetAssetActions");
	FToolMenuEntry& TextureEntry = TextureSection.AddDynamicEntry("AssetManagerEditorViewCommands", FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
	{
		UContentBrowserAssetContextMenuContext* Context = InSection.FindContext<UContentBrowserAssetContextMenuContext>();
		if (Context)
		{
			FToolUIActionChoice PixelateTextureAction(FExecuteAction::CreateLambda([Context]()
			{
				FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
				
				for(FAssetData SelectedAsset : Context->SelectedAssets)
				{
					UTexture* TextureAsset = Cast<UTexture>(SelectedAsset.GetAsset());
					if (TextureAsset && !TextureAsset->IsCompiling())
					{
						TextureAsset->Filter = TF_Nearest;
						TextureAsset->MipGenSettings = TMGS_NoMipmaps;
						TextureAsset->NeverStream = true;
						TextureAsset->CompressionSettings = TextureCompressionSettings::TC_EditorIcon;
						TextureAsset->LODGroup = TEXTUREGROUP_Pixels2D;
						TextureAsset->bUseNewMipFilter = false;
						UE_LOG(LogTemp, Log, TEXT("PixelTools: Texture pixelated succesfuly"));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("PixelTools warning: Texture not compile already, please wait and try again"));
					}
				}
			}));
			
			InSection.AddEntry(FToolMenuEntry::InitMenuEntry(FName("Pixelate"), FText::FromString("Pixelate Texture"), FText::FromString("Setup special settings to this texture to remove blurry effect in build"), FSlateIcon(FPixelateTextureStyle::Get().GetStyleSetName(), "PixelateTexture"), PixelateTextureAction));
		}
	}));
	
	UToolMenu* TargetMenu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.TextureRenderTarget2D");
	FToolMenuSection&  TargetSection = TargetMenu->FindOrAddSection("GetAssetActions");
	FToolMenuEntry& TargetEntry = TargetSection.AddDynamicEntry("AssetManagerEditorViewCommands", FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
	{
		UContentBrowserAssetContextMenuContext* Context = InSection.FindContext<UContentBrowserAssetContextMenuContext>();
		if (Context)
		{
			FToolUIActionChoice PixelateTargetAction(FExecuteAction::CreateLambda([Context]()
			{
				FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
				
				for(FAssetData SelectedAsset : Context->SelectedAssets)
				{
					UTextureRenderTarget2D* RenderTargetAsset = Cast<UTextureRenderTarget2D>(SelectedAsset.GetAsset());
					if(RenderTargetAsset && !RenderTargetAsset->IsCompiling())
					{
						RenderTargetAsset->Filter = TF_Nearest;
						RenderTargetAsset->MipGenSettings = TMGS_NoMipmaps;
						RenderTargetAsset->LODGroup = TEXTUREGROUP_Pixels2D;
						RenderTargetAsset->DownscaleOptions = ETextureDownscaleOptions::Unfiltered;
						RenderTargetAsset->NeverStream = true;
						RenderTargetAsset->bAutoGenerateMips = true;
						RenderTargetAsset->MipsSamplerFilter = TF_Nearest;
						UE_LOG(LogTemp, Log, TEXT("PixelTools: RenderTarget pixelated succesfuly"));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("PixelTools warning: RenderTarget not compile already, please wait and try again"));
					}
				}
			}));
			
			InSection.AddEntry(FToolMenuEntry::InitMenuEntry(FName("Pixelate"), FText::FromString("Pixelate RenderTarget"), FText::FromString("Setup special settings to this RenderTarget to remove blurry effect in build"), FSlateIcon(FPixelateTextureStyle::Get().GetStyleSetName(), "PixelateTexture"), PixelateTargetAction));
		}
	}));

	UToolMenu* ProjectSettigsMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.ViewportToolbar.PerformanceAndScalability");
	FToolMenuSection&  ProjectSettigsSection = ProjectSettigsMenu->FindOrAddSection("ProjectSettingsSection");
	ProjectSettigsSection.AddMenuEntry("Pixelate",
		LOCTEXT("PixelateProject", "Set pixelated settings"),
		LOCTEXT("ToolTip", "Set settings preferred for clear pixel grafics. Set PixelPerfect: GameInstance, GameUserSettings, GameViewportClient (if you want use custom, make childs from PixelPerfect classes)."),
		FSlateIcon(FPixelateTextureStyle::Get().GetStyleSetName(), "PixelateTexture"),
		FUIAction(
			FExecuteAction::CreateRaw(this, &FPixelToolsEditorModule::PixelateProject),
			FCanExecuteAction()
			)
			);
}

void FPixelToolsEditorModule::PixelateProject()
{
	if(URendererSettings* RendererSettings = GetMutableDefault<URendererSettings>())
	{
		RendererSettings->DefaultManualScreenPercentage = 100;
		RendererSettings->DefaultScreenPercentageDesktopMode = EScreenPercentageMode::BasedOnDisplayResolution;
		RendererSettings->DefaultScreenPercentageMobileMode = EScreenPercentageMode::BasedOnDisplayResolution;
		RendererSettings->DefaultFeatureAntiAliasing = AAM_None;
		RendererSettings->MobileAntiAliasing = EMobileAntiAliasingMethod::None;
		RendererSettings->bDefaultFeatureMotionBlur = false;
		RendererSettings->DynamicGlobalIllumination = EDynamicGlobalIlluminationMethod::None;
		RendererSettings->bDefaultFeatureAutoExposure = false;
		RendererSettings->DefaultFeatureAutoExposureBias = 0;
		RendererSettings->Reflections = EReflectionMethod::None;
		RendererSettings->ShadowMapMethod = EShadowMapMethod::ShadowMaps;
		RendererSettings->bDefaultFeatureBloom = false;
		RendererSettings->bTemporalUpsampling = false;
		RendererSettings->SaveConfig();
		RendererSettings->TryUpdateDefaultConfigFile();
		
		// UE 5.7+: Console variables must go in [ConsoleVariables] or [SystemSettings] section
		// Note: 0=None (crashes!), 1=Nearest (pixel-perfect)
		GConfig->SetInt(TEXT("ConsoleVariables"), TEXT("r.Upscale.Quality"), 1, GEngineIni);
		GConfig->SetInt(TEXT("ConsoleVariables"), TEXT("r.TemporalAA.Upsampling"), 0, GEngineIni);
		GConfig->SetInt(TEXT("ConsoleVariables"), TEXT("r.AntiAliasingMethod"), 0, GEngineIni);
		GConfig->Flush(false, GEngineIni);
	}

	if(UGeneralProjectSettings* GeneralSettings = GetMutableDefault<UGeneralProjectSettings>())
	{
		GeneralSettings->bAllowMaximize = false;
		GeneralSettings->bAllowMinimize = false;
		GeneralSettings->bAllowWindowResize = false;
		GeneralSettings->SaveConfig();
		GeneralSettings->TryUpdateDefaultConfigFile();
	}

	if(UInputSettings* InputSettings = GetMutableDefault<UInputSettings>())
	{
		InputSettings->bAltEnterTogglesFullscreen = false;
		InputSettings->bF11TogglesFullscreen = false;
		InputSettings->SaveConfig();
		InputSettings->TryUpdateDefaultConfigFile();
	}
	
	if (UEngine* EngineSettings = GetMutableDefault<UEngine>())
	{
		// Set GameUserSettings
		EngineSettings->GameUserSettingsClass = UPixelPerfectGameUserSettings::StaticClass();
		EngineSettings->GameUserSettingsClassName = FSoftClassPath(TEXT("/Script/PixelToolsRuntime.PixelPerfectGameUserSettings"));

		// Set GameViewportClient
		EngineSettings->GameViewportClientClass = UPixelPerfectViewportClient::StaticClass();
		EngineSettings->GameViewportClientClassName = FSoftClassPath(TEXT("/Script/PixelToolsRuntime.PixelPerfectViewportClient"));

		
		EngineSettings->SaveConfig();
		EngineSettings->TryUpdateDefaultConfigFile();
	}
	
	// Set GameInstance
	if (UGameMapsSettings* MapsSettings = GetMutableDefault<UGameMapsSettings>())
	{
		MapsSettings->GameInstanceClass = FSoftClassPath(TEXT("/Script/PixelToolsRuntime.PixelPerfectGameInstance"));
		MapsSettings->SaveConfig();
		MapsSettings->TryUpdateDefaultConfigFile();
	}

	// Call ReloadEditor Notify
	static TWeakPtr<SNotificationItem> GPixelNotify;
	
	FNotificationInfo Info(LOCTEXT("PixelToolsRestart", "Project settings changed. Please restart the editor."));
	Info.bFireAndForget = false;
	Info.ExpireDuration = 0.0f;
	
		Info.ButtonDetails.Add(
			FNotificationButtonInfo(
				LOCTEXT("RestartNow","Restart Now"),
				LOCTEXT("RestartNowTooltip","Restart now"),
				FSimpleDelegate::CreateLambda([]()
				{
					if (TSharedPtr<SNotificationItem> P = GPixelNotify.Pin())
					{
						P->ExpireAndFadeout();
					}
					FUnrealEdMisc::Get().RestartEditor(false);
				})
			)
		);

		Info.ButtonDetails.Add(
			FNotificationButtonInfo(
				LOCTEXT("RestartLater","Restart Later"),
				LOCTEXT("RestartLaterTooltip","Dismiss"),
				FSimpleDelegate::CreateLambda([]()
				{
					if (TSharedPtr<SNotificationItem> P = GPixelNotify.Pin())
					{
						P->ExpireAndFadeout();
					}
				})
			)
		);
	
		TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
		GPixelNotify = Notification;

		if (Notification.IsValid())
		{
			Notification->SetCompletionState(SNotificationItem::CS_Pending);
		}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPixelToolsEditorModule, PixelToolsEditor)