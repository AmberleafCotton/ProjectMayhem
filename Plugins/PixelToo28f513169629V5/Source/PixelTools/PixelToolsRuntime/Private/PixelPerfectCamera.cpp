//copyright Michael Bochkarev (Turbomorsh)


#include "PixelPerfectCamera.h"

#include "PixelPerfectCameraComponent.h"

// Sets default values
APixelPerfectCamera::APixelPerfectCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CaptureComponent = CreateDefaultSubobject<UPixelPerfectCameraComponent>(TEXT("PixelPerfectCapture"));
}

// Called when the game starts or when spawned
void APixelPerfectCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APixelPerfectCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

