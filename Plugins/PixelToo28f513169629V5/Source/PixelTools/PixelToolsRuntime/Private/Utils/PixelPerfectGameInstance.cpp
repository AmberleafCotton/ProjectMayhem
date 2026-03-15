//copyright Michael Bochkarev (Turbomorsh)


#include "Utils/PixelPerfectGameInstance.h"

#include "GameFramework/GameUserSettings.h"
#include "Utils/PixelPerfectViewportClient.h"
#include "Engine/Engine.h"

void UPixelPerfectGameInstance::OnStart()
{
	Super::OnStart();
	
	if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
	{
		Settings->SetVSyncEnabled(false);
		Settings->SetDynamicResolutionEnabled(false);
		
		if (Settings->GetLastConfirmedFullscreenMode() == EWindowMode::Type::Fullscreen)
		{
			if (UPixelPerfectViewportClient* PPGameViewport = Cast<UPixelPerfectViewportClient>(GEngine->GameViewport))
			{
				Settings->SetScreenResolution(PPGameViewport->GetCurrentDesktopSize());
			}
			else
			{
				Settings->SetScreenResolution(Settings->GetDesktopResolution());
			}
		}

		Settings->ApplyResolutionSettings(false);
		Settings->ApplyNonResolutionSettings();
		Settings->ApplySettings(false);
		Settings->SaveSettings();
	}
}
