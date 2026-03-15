//copyright Michael Bochkarev (Turbomorsh)

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "PixelPerfectCameraComponent.generated.h"

class UInterpComponent;
class UPixelRenderingWidgetBase;
class UCurveFloat;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScaleBlendingFinished);

/**
 * PixelPerfectCameraComponent manage screen percentage and ortho width for clear pixel graphics. 
 * Put on Variable "Resolution" Low resolution (For example: 1920:1080 / 3 -> 640:360. It automatically scales for other aspect ratios)
 */
UCLASS(hidecategories=(Mobility, Rendering, LOD), Blueprintable, ClassGroup=Camera, meta=(BlueprintSpawnableComponent))
class PIXELTOOLSRUNTIME_API UPixelPerfectCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	// Set default PixelPerfectCameraComponent settings
	void PixelPerfectCameraComponent();
	
	virtual void PostInitProperties() override;
	
	// Call on play started
	UFUNCTION()
	virtual void BeginPlay() override;

	// Call every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	// Find next or previous pixel perfect resolution based on DESKTOP resolution. (Desktop: 1920:1080)(Resolution: 640:360, scale -1 -> 960:540)
	UFUNCTION(BlueprintCallable, Category="Resolution")
	FIntPoint CalcResolutionDesktopBased(int Scale = 0);

	// Find next or previous pixel perfect resolution based on WINDOW resolution. (Window: 640:360)(Resolution: 640:360, scale -1 -> 1280:720)
	UFUNCTION(BlueprintCallable, Category="Resolution")
	FIntPoint CalcResolutionWindowBased(int Scale = 0);

	// If resolution incorect, recalculate it using division Desktop res to Camera res.
	UFUNCTION(BlueprintCallable, Category="Resolution")
	void RoundResolution();
	
	// Sets next or previous pixel perfect resolution based on DESKTOP resolution. (Desktop: 1920:1080)(Resolution: 640:360, scale -1 -> 960:540)
	UFUNCTION(BlueprintCallable, Category="Resolution")
	void ScaleResolution(int Scale = 1);

	// Sets next or previous pixel perfect resolution based on DESKTOP resolution with blend. Only once at a time. (Desktop: 1920:1080)(Resolution: 640:360, scale -1 -> 960:540)
	UFUNCTION(BlueprintCallable, Category="Resolution")
	void ScaleResolutionWithBlend(int Scale = 1, float Time = 1, int TargetFPS = 60);

	// Sets ortho width, based on resolution and pixel per unit, for fix pixel distortion
	UFUNCTION(BlueprintCallable, Category="Resolution")
	void SetPerfectOrthoWidth();

	// Snaps camera to pixel grid, based on pixel per unit
	UFUNCTION(BlueprintCallable, Category="Resolution")
	void SetPerfectPosition();

	// Apply Pixelated Resolution Settings
	void ApplyResolution();

protected:
	// Timer Functions
	
	// Call on BlendTimeLine playing
	UFUNCTION()
	void BlendUpdate(FIntPoint FinRes, float Time);

	// Call on BlendTimeLine finish playing
	UFUNCTION()
	void BlendFinished();

	// PPViewportClient link events
	
	void OnViewportResizeFinished();

	void OnViewportResizeFinishedLocal();

	void ResizeRequest();
	
public:
	// Set resolution for RenderTexture
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Resolution")
	FIntPoint Resolution = FIntPoint(640, 360);

	// PixelPerUnit
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Resolution")
	float PixelPerUnit = 1;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnScaleBlendingFinished OnScaleBlendingFinished;

protected:
	// Blend properties
	FTimerHandle BlendTimer;

	float TimerCout = 0;

	FVector LastPerfectPos = FVector::ZeroVector;
};
