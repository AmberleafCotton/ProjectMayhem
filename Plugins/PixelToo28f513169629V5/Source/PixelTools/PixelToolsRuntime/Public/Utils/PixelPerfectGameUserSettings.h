//copyright Michael Bochkarev (Turbomorsh)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "PixelPerfectGameUserSettings.generated.h"

// PixelPerfectGameUserSettings is toolset for save pixel perfect stats and management game window for clear pixel graphics.
UCLASS(config=GameUserSettings, configdonotcheckdefaults, Blueprintable)
class PIXELTOOLSRUNTIME_API UPixelPerfectGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Resolution")
	FIntPoint GetLastConfirmedFinalResolution() { return FinalResolution; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="WindowScaling")
	int32 GetLastConfirmedWindowScale() { return WindowScale; }

	// Save PixelPerfect resolution
	UFUNCTION(BlueprintCallable, Category="Resolution")
	void SetFinalResolution(const FIntPoint& InResolution);

	// Multiply window size to render small resolution in bigger window
	UFUNCTION(BlueprintCallable, Category="WindowScaling")
	void ScaleWindow(int32 InScale);

	// Set fullscreen mode so that it use as viewport size current screen size
	UFUNCTION(BlueprintCallable, Category="WindowScaling")
	void SetExclusiveFullscreenSafety();

	UFUNCTION(BlueprintCallable, Category="WindowScaling")
	// Set window size
	void ResizeWindow(const FIntPoint& InRes);

protected:

	UPROPERTY(Config)
	int32 WindowScale = 1;
	
	UPROPERTY(Config)
	FIntPoint FinalResolution = FIntPoint(0);
};
