// JY Sun All Rights Reserved


#include "HUD/GameEndOverlay.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UGameEndOverlay::NativeConstruct()
{
    Super::NativeConstruct();

    if (GoToMainMenu1)
        GoToMainMenu1->OnClicked.AddDynamic(this, &UGameEndOverlay::GoToMainMenu);
    if (GoToMainMenu2)
        GoToMainMenu2->OnClicked.AddDynamic(this, &UGameEndOverlay::GoToMainMenu);
}

void UGameEndOverlay::SetupGameEndOverlay(bool bIsWin)
{
    if (GameOverSwitcher)
    {
        GameOverSwitcher->SetActiveWidgetIndex(bIsWin ? 1 : 0);
    }
}

void UGameEndOverlay::GoToMainMenu()
{
    UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenu_Level"));
}


void UGameEndOverlay::ShowPlayerGameEnd(TSubclassOf<UGameEndOverlay> GameEndOverlayClass, bool bIsWin)
{
    UWorld* World = GetWorld();
    if(World)
    {
        APlayerController* Controller = World->GetFirstPlayerController();
        if(Controller && GameEndOverlayClass)
        {
            UGameEndOverlay* GameEndOverlay = CreateWidget<UGameEndOverlay>(Controller, GameEndOverlayClass);
            if(GameEndOverlay)
            {
                GameEndOverlay->AddToViewport();
                GameEndOverlay->SetupGameEndOverlay(bIsWin);
            }
        }
    }
}

