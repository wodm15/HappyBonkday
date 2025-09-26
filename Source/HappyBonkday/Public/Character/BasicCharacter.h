// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"

#include "BasicCharacter.generated.h"


class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;

class UAnimMontage;
class AItem;
class ASoul;
class ATreasure;
class UAnimMontage;
class UBasicOverlay;

UCLASS()
class HAPPYBONKDAY_API ABasicCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:

	ABasicCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hiiter) override;
	virtual void SetOverlappingItem(class AItem* Item) override;
	virtual void AddSouls(class ASoul* Soul) override;
	virtual void AddGold(class ATreasure* Treasure) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void Die_Implementation(const FVector& ImpactPoint) override;

	void InitializePlayerInput(APlayerController* PlayerController);
	void InitializeBasicOverlay(APlayerController* PlayerController);

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EKeyPressed(const FInputActionValue& Value);
	void Dodge(const FInputActionValue& Value);
	void Crounch(const FInputActionValue& Value);

	virtual void Attack() override;
	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;
	virtual void CrounchEnd() override;
	virtual void StandEnd() override;
	virtual bool CanAttack() override;

	void EquipWeapon(AWeapon* Weapon);
	void PlayEquipMontage(FName SectionName);
	void CheckWeaponType(AWeapon* Weapon);
	void CheckWeaponTypeBack(AWeapon* Weapon);
	bool HasEnoughStamina();
	bool CanDisarm();
	bool CanArm();
	void DisArm();
	void Arm(AWeapon* Weapon);


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
	UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = Input)
	UInputAction* DodgeAction;
	UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = Input)
	UInputAction* CrounchAction;



	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();



private:
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	bool IsOccupied();

	UPROPERTY(BlueprintReadWrite , meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;
	EStanceState StanceState = EStanceState::ESS_Standing;

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

	UPROPERTY()
	UBasicOverlay* BasicOverlay;

	void SetHUDHealth();
public:
	FORCEINLINE ECharacterState GetCharacterState() const {return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	FORCEINLINE EStanceState GetStanceState() const { return StanceState; }
};
