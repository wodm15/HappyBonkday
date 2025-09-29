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
protected:
    virtual void NativeConstruct() override;

public:
	void SetHealthProgressBar(float Percent);
	void SetStaminaProgressBar(float Percent);
	void SetGold(int32 Gold);
	void SetSouls(int32 Souls);

private:
	UPROPERTY(EditAnywhere)
	int32 GoldTarget = 10;
	UPROPERTY(EditAnywhere)
	int32 SoulsTarget = 10;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaProgressBar;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SoulsText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldTargetText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SoulsTargetText;

public:
	FORCEINLINE int32 GetGoldTarget() const {return GoldTarget; }
	FORCEINLINE int32 GetSoulsTarget() const {return SoulsTarget; }
};
