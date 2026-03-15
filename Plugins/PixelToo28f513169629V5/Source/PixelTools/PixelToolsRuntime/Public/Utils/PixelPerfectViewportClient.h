//copyright Michael Bochkarev (Turbomorsh)

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "TimerManager.h"

#include "PixelPerfectViewportClient.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnViewportResizeFinished);

// PixelPerfectViewportClient controls game window`s scale for clear pixel graphics.
UCLASS()
class PIXELTOOLSRUNTIME_API UPixelPerfectViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

public:
	FOnViewportResizeFinished ViewportResizeFinishedEvent;
	FOnViewportResizeFinished ViewportResizeFinishedEventLocal;
	
	virtual void Init(FWorldContext& WorldContext, UGameInstance* OwningGameInstance,
		bool bCreateNewAudioDevice) override;
	
	void SetMinimalWindowSize(FIntPoint MinimalScale);

	FIntPoint GetCurrentDesktopSize();

	void SetPerfectSize(bool bBroadcast = true);

	void ResizeNativeWindow(int InScale);

	void CenterWindow();

protected:
	
	void OnViewportResized(FViewport* InViewport, uint32 value);
	
	UFUNCTION()
	void OnResizeFinish();
	
	UPROPERTY()
	FTimerHandle ResizeDelay;

	UPROPERTY()
	FIntPoint NativeResolution = FIntPoint(0, 0);

	int32 WindowScaling = 0;
};
