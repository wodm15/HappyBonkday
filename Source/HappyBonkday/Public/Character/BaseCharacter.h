// JY Sun All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "CharacterTypes.h"
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



protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hiiter) override;
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	virtual void Attack();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	virtual bool CanAttack();
	bool IsAlive();
	virtual void Die(const FVector& ImpactPoint);
	void PlayHitReactMontage(const FName& SectionName);
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	void DisableCapsule();

	void PlayMontageSection(UAnimMontage* Montage , const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage , const TArray<FName>& SectionName);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	void StopAttackMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();
	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;
	UPROPERTY(BlueprintReadOnly , Category= Combat)
	AActor* CombatTarget;
	UPROPERTY(BlueprintReadOnly , Category= Combat)
	float WarpTargetDistance = 30.f;

	UPROPERTY(VisibleAnywhere , Category = Weapon)
	AWeapon* EquippedWeapon;


	UPROPERTY(BlueprintReadOnly)
	EDeathPos DeathPos;

	//Animation Montage
	UPROPERTY(EditDefaultsOnly , Category = Combats)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere , Category = Combat)
	TArray<FName> AttackMontageSections;
	UPROPERTY(EditAnywhere , Category = Combat)
	TArray<FName> DeathMontageSections;



private:
	//Sound
	UPROPERTY(EditAnywhere , Category = Combat)
	USoundBase* HitSound;

	//Effect
	UPROPERTY(EditAnywhere , Category = Combat)
	UParticleSystem* HitParticles;
	//Animation Montage
	UPROPERTY(EditDefaultsOnly , Category = Combat)
	UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly , Category = Combats)
	UAnimMontage* HitReactMontage;

public:
	FORCEINLINE EDeathPos GetDeathPos() const { return DeathPos; }
};
