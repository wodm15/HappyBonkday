// JY Sun All Rights Reserved


#include "HUD/BasicHUD.h"
#include "HUD/BasicOverlay.h"

void ABasicHUD::BeginPlay()
{
    Super::BeginPlay();
    UWorld* World = GetWorld();
    if(World)
    {
        APlayerController* Controller = World->GetFirstPlayerController();
        if(Controller && BasicOverlayClass)
        {
            BasicOverlay = CreateWidget<UBasicOverlay>(Controller, BasicOverlayClass);
            BasicOverlay->AddToViewport();
        }
    }

    
}
