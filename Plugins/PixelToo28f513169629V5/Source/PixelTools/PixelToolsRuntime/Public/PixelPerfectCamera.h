//copyright Michael Bochkarev (Turbomorsh)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PixelPerfectCamera.generated.h"

class UPixelPerfectCameraComponent;

UCLASS(hidecategories=(Collision, Attachment, Actor))
class PIXELTOOLSRUNTIME_API APixelPerfectCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APixelPerfectCamera();
	
	UPROPERTY(Category = DecalActor, VisibleAnywhere, BlueprintReadOnly)
	UPixelPerfectCameraComponent* CaptureComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
