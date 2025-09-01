// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"

#include "BasicCharacter.generated.h"


class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;

class UAnimMontage;
class AItem;
class UAnimMontage;

UCLASS()
class HAPPYBONKDAY_API ABasicCharacter : public ABaseCharacter
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
	virtual void Attack() override;

	//Play Anim Montage

	virtual void AttackEnd() override;
	virtual bool CanAttack() override;

	void PlayEquipMontage(FName SectionName);
	bool CanDisarm();
	bool CanArm();

	UFUNCTION(BlueprintCallable)
	void Disarm();
	UFUNCTION(BlueprintCallable)
	void Arm();
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

private:
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite , meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	//camera
	UPROPERTY(VisibleAnywhere)
    USpringArmComponent* SpringArm;
    UPROPERTY(VisibleAnywhere)
    UCameraComponent* ViewCamera;

	//equip
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;


	//Animation Montage
	UPROPERTY(EditDefaultsOnly , Category = Montages)
	UAnimMontage* EquipMontage;

public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE ECharacterState GetCharacterState() const {return CharacterState; }
};
