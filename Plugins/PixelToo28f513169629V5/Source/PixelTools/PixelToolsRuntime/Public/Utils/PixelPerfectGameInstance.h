//copyright Michael Bochkarev (Turbomorsh)

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PixelPerfectGameInstance.generated.h"

// PixelPerfectGameInstance helps game start with right settings for clear pixel graphics.
UCLASS()
class PIXELTOOLSRUNTIME_API UPixelPerfectGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	virtual void OnStart() override;
};
