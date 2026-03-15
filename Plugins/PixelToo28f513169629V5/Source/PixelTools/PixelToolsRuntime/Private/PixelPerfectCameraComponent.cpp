//copyright Michael Bochkarev (Turbomorsh)


#include "PixelPerfectCameraComponent.h"

#include "GameFramework/GameUserSettings.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"
#include "Slate/SceneViewport.h"
#include "Utils/PixelPerfectGameUserSettings.h"
#include "Utils/PixelPerfectViewportClient.h"
#include "Scalability.h"

void UPixelPerfectCameraComponent::PixelPerfectCameraComponent()
{
	//Set default values
	SetConstraintAspectRatio(false);
	bOverrideAspectRatioAxisConstraint = true;
	SetAspectRatioAxisConstraint(EAspectRatioAxisConstraint::AspectRatio_MaintainXFOV);
}

void UPixelPerfectCameraComponent::PostInitProperties()
{
	Super::PostInitProperties();
	//Set default values
	bAutoCalculateOrthoPlanes = false;
	PostProcessSettings.AutoExposureMinBrightness = 0;
	PostProcessSettings.AutoExposureMaxBrightness = 0;
	PostProcessSettings.HistogramLogMin = 0;
	PostProcessSettings.HistogramLogMax = 0;
	PostProcessSettings.VignetteIntensity = 0;
	PostProcessSettings.ExpandGamut = 0;
	PostProcessSettings.ToneCurveAmount = 0;
	PostProcessSettings.MotionBlurAmount = 0;
	PostProcessSettings.MotionBlurMax = 0;
	PostProcessSettings.MotionBlurTargetFPS = 0;
	PostProcessSettings.MotionBlurPerObjectSize = 0;
	SetProjectionMode(ECameraProjectionMode::Orthographic);
}

void UPixelPerfectCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	// Basic AA/blur disables (pixel-perfect rendering settings handled by PlayerController)
	if (GEngine)
	{
		GEngine->Exec(GetWorld(), TEXT("r.TemporalAA.Upsampling 1"));
		GEngine->Exec(GetWorld(), TEXT("r.AntiAliasingMethod 0"));
		GEngine->Exec(GetWorld(), TEXT("r.PostProcessAAQuality 0"));
		GEngine->Exec(GetWorld(), TEXT("r.MotionBlurQuality 0"));
	}
	
	if (UPixelPerfectViewportClient* PPGameViewport = Cast<UPixelPerfectViewportClient>(GEngine->GameViewport))
	{
		if (IsActive())
		PPGameViewport->SetMinimalWindowSize(CalcResolutionDesktopBased());
		PPGameViewport->ViewportResizeFinishedEvent.AddUObject(this, &UPixelPerfectCameraComponent::OnViewportResizeFinished);
		PPGameViewport->ViewportResizeFinishedEventLocal.AddUObject(this, &UPixelPerfectCameraComponent::OnViewportResizeFinishedLocal);
	}
}

void UPixelPerfectCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FIntPoint UPixelPerfectCameraComponent::CalcResolutionDesktopBased(int Scale)
{
	if (UPixelPerfectViewportClient* PPGameViewport = Cast<UPixelPerfectViewportClient>(GEngine->GameViewport))
	{
		FIntPoint DesktopSize = PPGameViewport->GetCurrentDesktopSize();
		int CurScale = DesktopSize.X / Resolution.X;

		int OutputResX;
		int OutputResY;
		
		if ((CurScale + Scale) > 0 && (CurScale + Scale) < 40)
		{
			OutputResX = DesktopSize.X / (CurScale + Scale);
			OutputResY = DesktopSize.Y / (CurScale + Scale);
			
			return(FIntPoint(OutputResX, OutputResY));	
		}
		else { return FIntPoint(Resolution.X, Resolution.Y); }
	}
	else { return FIntPoint(Resolution.X, Resolution.Y); }
}

FIntPoint UPixelPerfectCameraComponent::CalcResolutionWindowBased(int Scale)
{
	if (UPixelPerfectViewportClient* PPGameViewport = Cast<UPixelPerfectViewportClient>(GEngine->GameViewport))
	{
		FIntPoint WindowSize = PPGameViewport->GetGameViewport()->GetSizeXY();
		int CurScale = WindowSize.X / Resolution.X;

		int OutputResX;
		int OutputResY;

		if ((CurScale + Scale) > 0 && (CurScale + Scale) < 40)
		{
			OutputResX = WindowSize.X / (CurScale + Scale);
			OutputResY = WindowSize.Y / (CurScale + Scale);
			
			return(FIntPoint(OutputResX, OutputResY));	
		}
		else { return FIntPoint(Resolution.X, Resolution.Y); }
	}
	else { return FIntPoint(Resolution.X, Resolution.Y); }
}

void UPixelPerfectCameraComponent::RoundResolution()
{
	Resolution = CalcResolutionDesktopBased();
	ApplyResolution();
}

void UPixelPerfectCameraComponent::ScaleResolution(int Scale)
{
	if (UPixelPerfectViewportClient* PPGameViewport = Cast<UPixelPerfectViewportClient>(GEngine->GameViewport))
	{
		FIntPoint DesktopSize = PPGameViewport->GetCurrentDesktopSize();
		int OldScale = DesktopSize.X / Resolution.X;
		int NewScale = OldScale + Scale;
		//UE_LOG(LogTemp, Warning, TEXT("PixelPerfectCam: ScaleResolution(%d) — OldScale=%d, NewScale=%d, Desktop=%dx%d, OldRes=%dx%d"),
		//	Scale, OldScale, NewScale, DesktopSize.X, DesktopSize.Y, Resolution.X, Resolution.Y);
	}

	Resolution = CalcResolutionDesktopBased(Scale);

	ApplyResolution();

	if (UPixelPerfectViewportClient* PPGameViewport = Cast<UPixelPerfectViewportClient>(GEngine->GameViewport))
	{
		FIntPoint DesktopSize = PPGameViewport->GetCurrentDesktopSize();
		int CurScale = DesktopSize.X / Resolution.X;
		//UE_LOG(LogTemp, Warning, TEXT("PixelPerfectCam: → Result — CurScale=%d, NewRes=%dx%d, OrthoWidth=%.0f, PPU=%.2f, ScreenPct=%.1f%%"),
		//	CurScale, Resolution.X, Resolution.Y, OrthoWidth, PixelPerUnit,
		//	(float)Resolution.X / (float)DesktopSize.X * 100.f);
	}

	ResizeRequest();
}

void UPixelPerfectCameraComponent::ScaleResolutionWithBlend(int Scale, float Time, int TargetFPS)
{
	if(!BlendTimer.IsValid() && CalcResolutionDesktopBased(Scale).X != 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(BlendTimer);
		const float FPS = TargetFPS;
		const float Rate = 1 / FPS;
    	FIntPoint TimlessResScaled = CalcResolutionDesktopBased(Scale);
    	const FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &UPixelPerfectCameraComponent::BlendUpdate, TimlessResScaled, Time);
    	GetWorld()->GetTimerManager().SetTimer(BlendTimer, Delegate, Rate, true);
    }	
}

void UPixelPerfectCameraComponent::SetPerfectOrthoWidth()
{
	OrthoWidth = Resolution.X / PixelPerUnit;
}

void UPixelPerfectCameraComponent::SetPerfectPosition()
{
    FVector CameraLoc = GetOwner()->GetActorLocation();

    if (CameraLoc.X == 0.f && CameraLoc.Y == 0.f)
        return;

    if (CameraLoc == LastPerfectPos)
        return;

    const float GridValue = 1 / PixelPerUnit;

    CameraLoc.X = FMath::GridSnap(CameraLoc.X, GridValue);
    CameraLoc.Y = FMath::GridSnap(CameraLoc.Y, GridValue);
    CameraLoc.Z = FMath::GridSnap(CameraLoc.Z, GridValue);

    CameraLoc.X += 0.5f * GridValue;
    CameraLoc.Y += 0.5f * GridValue;
    CameraLoc.Z += 0.5f * GridValue;

    GetOwner()->SetActorLocation(CameraLoc);
    LastPerfectPos = CameraLoc;
}

void UPixelPerfectCameraComponent::BlendUpdate(FIntPoint FinRes, float Time)
{
	const int ResX = UKismetMathLibrary::FTrunc(UKismetMathLibrary::FInterpTo(Resolution.X, FinRes.X, TimerCout, 1 / Time));
	const int ResY = UKismetMathLibrary::FTrunc(UKismetMathLibrary::FInterpTo(Resolution.Y, FinRes.Y, TimerCout, 1 / Time));
	Resolution = FIntPoint(ResX, ResY);

	ApplyResolution();
	
	TimerCout += 0.1 / Time;
	if(TimerCout >= 1)
	{
		BlendFinished();
	}
}

void UPixelPerfectCameraComponent::BlendFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(BlendTimer);
	RoundResolution();
	TimerCout = 0;

	OnScaleBlendingFinished.Broadcast();
	ResizeRequest();
}

void UPixelPerfectCameraComponent::ApplyResolution()
{
	if (!IsActive())
		return;
	
	if (UGameUserSettings* GameSettings = GEngine->GetGameUserSettings())
	{
		float WindowResX = GEngine->GameViewport->GetGameViewport()->GetSizeXY().X;
		float TimlessResX = Resolution.X;
		
		GameSettings->SetResolutionScaleValueEx((TimlessResX / WindowResX) * 100);
		GameSettings->ApplyResolutionSettings(false);
		GameSettings->ApplyNonResolutionSettings();

		if (UPixelPerfectViewportClient* PPGameViewport = Cast<UPixelPerfectViewportClient>(GEngine->GameViewport))
		{
			PPGameViewport->SetMinimalWindowSize(Resolution);
		}

		if (UPixelPerfectGameUserSettings* PPGameSettings = Cast<UPixelPerfectGameUserSettings>(GameSettings))
		{
			PPGameSettings->SetFinalResolution(Resolution);
		}
	}

	SetPerfectOrthoWidth();
}

void UPixelPerfectCameraComponent::OnViewportResizeFinished()
{
	if (!IsActive())
		return;
	RoundResolution();
	SetPerfectPosition();
}

void UPixelPerfectCameraComponent::OnViewportResizeFinishedLocal()
{
	if (!IsActive())
		return;
	if (UGameUserSettings* GameSettings = GEngine->GetGameUserSettings())
	{
		float WindowResX = GEngine->GameViewport->GetGameViewport()->GetSizeXY().X;
		float TimlessResX = CalcResolutionWindowBased().X;
			
		GameSettings->SetResolutionScaleValueEx((TimlessResX / WindowResX) * 100);
		GameSettings->ApplyResolutionSettings(false);
		GameSettings->ApplyNonResolutionSettings();
	}
}

void UPixelPerfectCameraComponent::ResizeRequest()
{
	if (!IsActive())
		return;
	if (UPixelPerfectViewportClient* PPGameViewport = Cast<UPixelPerfectViewportClient>(GEngine->GameViewport))
	{
		if (UGameUserSettings* GameSettings = GEngine->GetGameUserSettings())
		{
			if (GameSettings->GetFullscreenMode() == EWindowMode::Type::Windowed)
			{
				PPGameViewport->SetPerfectSize(false);
			}
		}
	}
}
