// JY Sun All Rights Reserved


#include "UI/FrontendPlayerController.h"
#include "Kismet/GamePlayStatics.h"
#include "Camera/CameraActor.h"


void AFrontendPlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

    TArray<AActor*> FoundCameras;
    UGameplayStatics::GetAllActorsOfClassWithTag(this, ACameraActor::StaticClass() , FName("Default"), FoundCameras);

    if(!FoundCameras.IsEmpty())
    {
        SetViewTarget(FoundCameras[0]);
    }
}
