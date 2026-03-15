//copyright Michael Bochkarev (Turbomorsh)


#include "Utils/PixelPerfectGameUserSettings.h"

#include "Utils/PixelPerfectViewportClient.h"
#include "Engine/Engine.h"

void UPixelPerfectGameUserSettings::SetFinalResolution(const FIntPoint&  InResolution)
{
	FinalResolution = InResolution;
	SaveSettings();
}

void UPixelPerfectGameUserSettings::ScaleWindow(int32 InScale)
{
	UE_LOG(LogTemp, Warning, TEXT("PixelTools: ScaleWindow called with scale %d"), InScale);
	UE_LOG(LogTemp, Warning, TEXT("PixelTools: FullscreenMode = %d (0=Fullscreen, 1=WindowedFullscreen, 2=Windowed)"), (int32)GetFullscreenMode());
	
	if (GetFullscreenMode() == EWindowMode::Type::Windowed)
	{
		int32 ClampedScale = FMath::Max(InScale, 1);
		UPixelPerfectViewportClient* PPGameViewport = Cast<UPixelPerfectViewportClient>(GEngine->GameViewport);
		
		UE_LOG(LogTemp, Warning, TEXT("PixelTools: PPGameViewport valid = %s"), PPGameViewport ? TEXT("true") : TEXT("false"));
		
		if (PPGameViewport)
		{
			if (GetFullscreenMode() == EWindowMode::Type::Windowed)
			{
				WindowScale = ClampedScale;
				SaveSettings();
		
				UE_LOG(LogTemp, Warning, TEXT("PixelTools: Calling ResizeNativeWindow with scale %d"), ClampedScale);
				PPGameViewport->ResizeNativeWindow(ClampedScale);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PixelTools: GEngine->GameViewport is NOT a PixelPerfectViewportClient! Check Project Settings."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PixelTools: ScaleWindow ignored - not in Windowed mode"));
	}
}

void UPixelPerfectGameUserSettings::SetExclusiveFullscreenSafety()
{
	if (UPixelPerfectViewportClient* PPGameViewport = Cast<UPixelPerfectViewportClient>(GEngine->GameViewport))
	{
		SetScreenResolution(PPGameViewport->GetCurrentDesktopSize());
	}
	else
	{
		SetScreenResolution(GetDesktopResolution());
	}
	ApplySettings(false);
	SetFullscreenMode(EWindowMode::Type::Fullscreen);
	ApplySettings(false);
	SaveSettings();
}

void UPixelPerfectGameUserSettings::ResizeWindow(const FIntPoint& InRes)
{
	if (GetFullscreenMode() == EWindowMode::Type::Windowed)
	{
		SetFinalResolution(InRes);
		SetScreenResolution(InRes);
	}
}
