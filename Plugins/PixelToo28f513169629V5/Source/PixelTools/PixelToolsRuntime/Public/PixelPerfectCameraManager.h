//copyright Michael Bochkarev (Turbomorsh)

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "PixelPerfectCameraManager.generated.h"

//A PixelPerfectCameraManagerCore makes C++ logic support and base PixelPerfect logic. Use PixelPerfectCameraManager to your project
UCLASS()
class PIXELTOOLSRUNTIME_API APixelPerfectCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	//Sets default values
	virtual void PostInitProperties() override;

	//On begin play
protected:
	virtual void BeginPlay() override;

public:
	//Pixel per units from ScenePixelPerfectCapture2D
	UPROPERTY(BlueprintReadWrite, Category = "PixelPerUnit")
	float PixelsPerUnits = 1.0f;
	
	//Snaps camera position every frame, for fix pixel distortion
	virtual void DoUpdateCamera(float DeltaTime) override;

private:
	FVector LastPerfectPos = FVector::ZeroVector;
};
