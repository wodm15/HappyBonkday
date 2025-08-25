// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"

#include "BasicAnimInstance.generated.h"

class ABasicCharacter;
class UCharacterMovementComponent;

UCLASS()
class HAPPYBONKDAY_API UBasicAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public: 
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	ABasicCharacter* BasicCharacter;
	UPROPERTY(BlueprintReadOnly , category = Movement)
	UCharacterMovementComponent* BasicCharacterMovement;
	UPROPERTY(BlueprintReadOnly , category = Movement)
	float GroundSpeed;
	UPROPERTY(BlueprintReadOnly , category = Movement)
	bool IsFalling;
	UPROPERTY(BlueprintReadOnly , category = "Movement | Character State")
	ECharacterState CharacterState;
};
