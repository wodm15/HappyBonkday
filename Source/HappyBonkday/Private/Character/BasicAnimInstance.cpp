// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BasicAnimInstance.h"
#include "Character/BasicCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBasicAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    BasicCharacter = Cast<ABasicCharacter>(TryGetPawnOwner());
    if(BasicCharacter)
    {
        BasicCharacterMovement = BasicCharacter->GetCharacterMovement();
    }
}


void UBasicAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);
    if(BasicCharacterMovement)
    {
        GroundSpeed = UKismetMathLibrary::VSizeXY(BasicCharacterMovement->Velocity);
        IsFalling = BasicCharacterMovement->IsFalling();
        CharacterState = BasicCharacter->GetCharacterState();
        ActionState = BasicCharacter->GetActionState();
        DeathPos = BasicCharacter->GetDeathPos();
    }
}
