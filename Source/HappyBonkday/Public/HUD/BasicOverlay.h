// JY Sun All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BasicOverlay.generated.h"

/**
 * 
 */
UCLASS()
class HAPPYBONKDAY_API UBasicOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthProgressBar(float Percent);
	void SetStaminaProgressBar(float Percent);
	void SetGold(int32 Gold);
	void SetSouls(int32 Souls);

private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaProgressBar;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SoulsText;
};
