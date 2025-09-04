// JY Sun All Rights Reserved


#include "HUD/BasicOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void UBasicOverlay::SetHealthProgressBar(float Percent)
{
    if(HealthProgressBar)
    {
        HealthProgressBar->SetPercent(Percent);
    }
}


void UBasicOverlay::SetStaminaProgressBar(float Percent)
{
    if(StaminaProgressBar)
    {
        StaminaProgressBar->SetPercent(Percent);
    }
}


void UBasicOverlay::SetGold(int32 Gold)
{
    if(GoldText)
    {
        const FString GoldString = FString::Printf(TEXT("%d") , Gold );
        const FText Text = FText::FromString(GoldString);
        GoldText->SetText( Text );
    }
}

void UBasicOverlay::SetSouls(int32 Souls)
{
    if(SoulsText)
    {
        const FString SoulsString = FString::Printf(TEXT("%d") , Souls );
        const FText Text = FText::FromString(SoulsString);
        SoulsText->SetText( Text );
    }
}
