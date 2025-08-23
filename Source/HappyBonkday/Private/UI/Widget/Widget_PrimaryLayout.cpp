// JY Sun All Rights Reserved

#include "UI/Widget/Widget_PrimaryLayout.h"

UCommonActivatableWidgetContainerBase* UWidget_PrimaryLayout::FindWidgetStackByTag(const FGameplayTag& InTag) const
{
    checkf(RegisteredWidgetStackMap.Contains(InTag), TEXT("can not find widget stack with tag %s"), *InTag.ToString());

    return RegisteredWidgetStackMap.FindRef(InTag);
}


void UWidget_PrimaryLayout::RegisterWidgetStack(UPARAM(meta = (Categories = "Frontend.WidgetStack"))FGameplayTag InStackTag, UCommonActivatableWidgetContainerBase* InStack)
{
    if (!IsDesignTime())
    {
        if (!RegisteredWidgetStackMap.Contains(InStackTag))
        {
            RegisteredWidgetStackMap.Add(InStackTag, InStack);
        }
    }
}
