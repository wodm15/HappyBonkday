// JY Sun All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameEndOverlay.generated.h"

class UWidgetSwitcher;
class UButton;

UCLASS()
class HAPPYBONKDAY_API UGameEndOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetupGameEndOverlay(bool bIsWin);

protected:
	virtual void NativeConstruct() override;
private:
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* GameOverSwitcher;
	UPROPERTY(meta = (BindWidget))
	UButton* GoToMainMenu1;
	UPROPERTY(meta = (BindWidget))
	UButton* GoToMainMenu2;

    UFUNCTION()
    void GoToMainMenu();
    UFUNCTION()
    void ShowPlayerGameEnd(TSubclassOf<UGameEndOverlay> GameEndOverlayClass, bool bIsWin);
};
