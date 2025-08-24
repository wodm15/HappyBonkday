// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "InputActionValue.h"
#include "CharacterTypes.h"

#include "BasicCharacter.generated.h"


class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;

class UAnimMontage;
class AItem;

UCLASS()
class HAPPYBONKDAY_API ABasicCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ABasicCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:

	virtual void BeginPlay() override;

	//input
	UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = Input)
	UInputMappingContext* SlashContext;

	UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = Input)
	UInputAction* MovementAction;
	UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = Input)
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = Input)
	UInputAction* EKeyPressedAction;
	UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = Input)
	UInputAction* AttackAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EKeyPressed(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);

private:

	//camera
	UPROPERTY(VisibleAnywhere)
    USpringArmComponent* SpringArm;
    UPROPERTY(VisibleAnywhere)
    UCameraComponent* ViewCamera;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	//FORCEINLINE ECharacterState GetCharacterState() const {return CharacterState; }
};
