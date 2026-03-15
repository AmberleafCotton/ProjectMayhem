//copyright Michael Bochkarev (Turbomorsh)


#include "PixelPerfectCameraManager.h"

#include "PixelPerfectCameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void APixelPerfectCameraManager::PostInitProperties()
{
	Super::PostInitProperties();
	bIsOrthographic = true;
	DefaultOrthoWidth = 640;
}

void APixelPerfectCameraManager::BeginPlay()
{
	Super::BeginPlay();
	
	UPixelPerfectCameraComponent* VTarget = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetViewTarget()->GetComponentByClass<UPixelPerfectCameraComponent>();
	if (VTarget)
	{
		PixelsPerUnits = VTarget->PixelPerUnit;
	}
}

void APixelPerfectCameraManager::DoUpdateCamera(float DeltaTime)
{
    // Update the camera
    Super::DoUpdateCamera(DeltaTime);

    FMinimalViewInfo CameraCachePOV = GetCameraCacheView();
    FVector CameraLoc = CameraCachePOV.Location;

    if (CameraCachePOV.Location.X == 0.f && CameraCachePOV.Location.Y == 0.f)
        return;

    if (CameraLoc.X == 0.f && CameraLoc.Y == 0.f)
        return;

    if (CameraLoc == LastPerfectPos)
        return;

    const float GridValue = 1 / PixelsPerUnits;

    CameraLoc.X = FMath::GridSnap(CameraLoc.X, GridValue);
    CameraLoc.Y = FMath::GridSnap(CameraLoc.Y, GridValue);
    CameraLoc.Z = FMath::GridSnap(CameraLoc.Z, GridValue);

    CameraLoc.X += 0.5f * GridValue;
    CameraLoc.Y += 0.5f * GridValue;
    CameraLoc.Z += 0.5f * GridValue;

    CameraCachePOV.Location.X = CameraLoc.X;
    CameraCachePOV.Location.Y = CameraLoc.Y;
    CameraCachePOV.Location.Z = CameraLoc.Z;

    FillCameraCache(CameraCachePOV);

    LastPerfectPos = CameraLoc;
}
