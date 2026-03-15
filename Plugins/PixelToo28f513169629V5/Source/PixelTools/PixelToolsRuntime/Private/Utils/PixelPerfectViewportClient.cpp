//copyright Michael Bochkarev (Turbomorsh)


#include "Utils/PixelPerfectViewportClient.h"

#include "GameFramework/GameUserSettings.h"
#include "Slate/SceneViewport.h"
#include "Utils/PixelPerfectGameUserSettings.h"
#include "GenericPlatform/GenericApplication.h"
#include "Widgets/SWindow.h"
#include "Engine/Engine.h"
#include "Scalability.h"

void UPixelPerfectViewportClient::Init(FWorldContext& WorldContext, UGameInstance* OwningGameInstance,
                                       bool bCreateNewAudioDevice)
{
	Super::Init(WorldContext, OwningGameInstance, bCreateNewAudioDevice);
	
	FViewport::ViewportResizedEvent.AddUObject(this, &UPixelPerfectViewportClient::OnViewportResized);
}

void UPixelPerfectViewportClient::SetMinimalWindowSize(FIntPoint MinimalScale)
{
	if (TSharedPtr<SWindow> SWindow = GetWindow())
	{
		NativeResolution = MinimalScale;
		FWindowSizeLimits SizeLimits;
		SizeLimits.SetMinWidth(MinimalScale.X);
		SizeLimits.SetMinHeight(MinimalScale.Y);
		SWindow->SetSizeLimits(SizeLimits);
	}
}

void UPixelPerfectViewportClient::OnViewportResized(FViewport* InViewport, uint32 value)
{
	if (ResizeDelay.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ResizeDelay);
	}
	
	FTimerDelegate FlyingDelegate;
	FlyingDelegate.BindUFunction(this, "OnResizeFinish");
	GetWorld()->GetTimerManager().SetTimer(ResizeDelay, FlyingDelegate, 0.2f, false);
}

FIntPoint UPixelPerfectViewportClient::GetCurrentDesktopSize()
{
	if (TSharedPtr<SWindow> SlateWindow = GetWindow())
	{
		FDisplayMetrics DisplayMetrics;
		FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);
		
		FGeometry WinGeometry = SlateWindow->GetWindowGeometryInScreen();
		FSlateRect WindowRect = WinGeometry.GetLayoutBoundingRect();

		for (const FMonitorInfo& Monitor : DisplayMetrics.MonitorInfo)
		{
			FSlateRect MonitorRect(
				Monitor.DisplayRect.Left,
				Monitor.DisplayRect.Top,
				Monitor.DisplayRect.Right,
				Monitor.DisplayRect.Bottom
			);

			if (FSlateRect::DoRectanglesIntersect(WindowRect, MonitorRect))
			{
				return FIntPoint(Monitor.NativeWidth, Monitor.NativeHeight);
			}
		}
		
		return FIntPoint(DisplayMetrics.PrimaryDisplayWidth, DisplayMetrics.PrimaryDisplayHeight);
	}
	else
	{
		return FIntPoint(0, 0);
	}
}

void UPixelPerfectViewportClient::SetPerfectSize(bool bBroadcast)
{
#if WITH_EDITOR
		
		int32 WindowSizeX = GetWindow()->GetSizeInScreen().X;
		int32 ViewportSizeX = GetGameViewport()->GetSizeXY().X;

		if (WindowSizeX - ViewportSizeX > 15)
		{
			ViewportResizeFinishedEvent.Broadcast();
			return;
		}
		
#endif
	
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	if (Settings && GetWindow().IsValid())
	{
		if (Settings->GetFullscreenMode() == EWindowMode::Type::Fullscreen || Settings->GetFullscreenMode() == EWindowMode::Type::WindowedFullscreen)
		{
			if (bBroadcast)
				ViewportResizeFinishedEvent.Broadcast();
			
			return;
		}

		CenterWindow();
		
		if (UPixelPerfectGameUserSettings* PPGameSettings = Cast<UPixelPerfectGameUserSettings>(Settings))
		{
			FIntPoint NativeWindowSize = NativeResolution.X > 0 ? NativeResolution : PPGameSettings->GetLastConfirmedFinalResolution();
			if (NativeWindowSize != FIntPoint(0))
			{
				int32 WindowScale = WindowScaling == 0 ? PPGameSettings->GetLastConfirmedWindowScale() : WindowScaling;
				if (WindowScale != 1)
				{
					ResizeNativeWindow(WindowScale);
					if (bBroadcast)
					ViewportResizeFinishedEvent.Broadcast();
					return;
				}
				else
				{
					GetWindow()->Resize(NativeWindowSize);
					if (bBroadcast)
					ViewportResizeFinishedEvent.Broadcast();
					return;
				}
			}
			else
			{
				GetWindow()->Resize(GetCurrentDesktopSize());
				if (bBroadcast)
				ViewportResizeFinishedEvent.Broadcast();
				return;
			}
		}
		
		GetWindow()->Resize(GetCurrentDesktopSize());
		if (bBroadcast)
			ViewportResizeFinishedEvent.Broadcast();
	}
}

void UPixelPerfectViewportClient::ResizeNativeWindow(int InScale)
{
	UE_LOG(LogTemp, Warning, TEXT("PixelTools: ResizeNativeWindow called with scale %d"), InScale);
	
	WindowScaling = InScale;
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	if (UPixelPerfectGameUserSettings* PPGameSettings = Cast<UPixelPerfectGameUserSettings>(Settings))
	{
		UE_LOG(LogTemp, Warning, TEXT("PixelTools: PPGameSettings valid"));
		UE_LOG(LogTemp, Warning, TEXT("PixelTools: NativeResolution = %d x %d"), NativeResolution.X, NativeResolution.Y);
		UE_LOG(LogTemp, Warning, TEXT("PixelTools: LastConfirmedFinalResolution = %d x %d"), 
			PPGameSettings->GetLastConfirmedFinalResolution().X, 
			PPGameSettings->GetLastConfirmedFinalResolution().Y);
		
		if (TSharedPtr<SWindow> SWindow = GetWindow())
		{
			// Window Scale Logic
			FIntPoint FinalRes = NativeResolution.X > 0 ? NativeResolution : PPGameSettings->GetLastConfirmedFinalResolution();
			
			UE_LOG(LogTemp, Warning, TEXT("PixelTools: FinalRes (used for scaling) = %d x %d"), FinalRes.X, FinalRes.Y);
			
			if (FinalRes.X == 0 || FinalRes.Y == 0)
			{
				UE_LOG(LogTemp, Error, TEXT("PixelTools: FinalRes is ZERO! Window cannot be scaled. Set camera Resolution first."));
				return;
			}
			
			int32 NewSizeX = FMath::Clamp(FinalRes.X * InScale, FinalRes.X, GetCurrentDesktopSize().X);
			int32 NewSizeY = FMath::Clamp(FinalRes.Y * InScale, FinalRes.Y, GetCurrentDesktopSize().Y);

			UE_LOG(LogTemp, Warning, TEXT("PixelTools: Calculated new window size = %d x %d"), NewSizeX, NewSizeY);

			// Window Resize Center Logic
			FVector2D WinCurrentSize = SWindow->GetSizeInScreen();
			FVector2D AdditionalPixels = WinCurrentSize - SWindow->GetClientSizeInScreen();

			FVector2D WinNewSize = FVector2D(
				FMath::Clamp(FinalRes.X * InScale + AdditionalPixels.X, FinalRes.X, GetCurrentDesktopSize().X),
				FMath::Clamp(FinalRes.Y * InScale + AdditionalPixels.Y, FinalRes.Y, GetCurrentDesktopSize().Y));

			FVector2D WinPos = SWindow->GetPositionInScreen();

			FVector2D CurrentCenter = FVector2D(
				WinPos.X + WinCurrentSize.X / 2,
				WinPos.Y + WinCurrentSize.Y / 2);

			FVector2D NewCenter = FVector2D(
				WinPos.X + WinNewSize.X / 2,
				WinPos.Y + WinNewSize.Y / 2);

			FVector2D CenterDelta = CurrentCenter - NewCenter;

			SWindow->MoveWindowTo(WinPos + CenterDelta);
			
			// Window Scale Logic
			SWindow->Resize(FIntPoint(NewSizeX, NewSizeY));
			UE_LOG(LogTemp, Warning, TEXT("PixelTools: Window resized successfully to %d x %d"), NewSizeX, NewSizeY);
			ViewportResizeFinishedEventLocal.Broadcast();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PixelTools: GetWindow() returned nullptr!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PixelTools: GameUserSettings is NOT a PixelPerfectGameUserSettings!"));
	}
}

void UPixelPerfectViewportClient::CenterWindow()
{
	FVector2D WindowPose = GetWindow()->GetPositionInScreen();
	if (WindowPose.X < 10.f && WindowPose.Y < 10.f)
	{
		FIntPoint DesktopSize = GetCurrentDesktopSize();
		FVector2D WindowSize = GetWindow()->GetSizeInScreen();

		FVector2D NewPos(
				   (DesktopSize.X  - WindowSize.X) / 2.0f,
				   (DesktopSize.Y - WindowSize.Y) / 2.0f
			   );

		GetWindow()->MoveWindowTo(NewPos);
	}
}

void UPixelPerfectViewportClient::OnResizeFinish()
{
	SetPerfectSize();
}
