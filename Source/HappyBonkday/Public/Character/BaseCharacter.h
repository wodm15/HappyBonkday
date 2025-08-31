// JY Sun All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"

#include "BaseCharacter.generated.h"

class UAttributeComponent;
class AWeapon;
class UAnimMontage;

UCLASS()
class HAPPYBONKDAY_API ABaseCharacter : public ACharacter , public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

protected:
	virtual void BeginPlay() override;
	//Attribute
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	virtual void Attack();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	virtual bool CanAttack();
	virtual void PlayAttackMontage();

	virtual void Die(const FVector& ImpactPoint);

	//Play Anim Montage
	void PlayHitReactMontage(const FName& SectionName);
	void DirectionalHitReact(const FVector& ImpactPoint);

	UPROPERTY(VisibleAnywhere , Category = Weapon)
	AWeapon* EquippedWeapon;

	//Animation Montage
	UPROPERTY(EditDefaultsOnly , Category = Montages)
	UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly , Category = Montages)
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly , Category = Montages)
	UAnimMontage* DeathMontage;

	//Sound
	UPROPERTY(EditAnywhere , Category = Sounds)
	USoundBase* HitSound;

	//Effect
	UPROPERTY(EditAnywhere , Category = VisibleEffect)
	UParticleSystem* HitParticles;
};
