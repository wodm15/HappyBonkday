// JY Sun All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BasicHUD.generated.h"

/**
 * 
 */
UCLASS()
class HAPPYBONKDAY_API ABasicHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly , Category= Basic)
	TSubclassOf<class UBasicOverlay> BasicOverlayClass;

	UPROPERTY()
	UBasicOverlay* BasicOverlay;

public:
	FORCEINLINE UBasicOverlay* GetBasicOverlay() const { return BasicOverlay; }
};
