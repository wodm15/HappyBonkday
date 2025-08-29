// JY Sun All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Character/CharacterTypes.h"

#include "Enemy.generated.h"

class UAnimMontage;
class UAttributeComponent;
class UHealthBarComponent;
class AAIController;

UCLASS()
class HAPPYBONKDAY_API AEnemy : public ACharacter , public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
protected:
	virtual void BeginPlay() override;

	//Play Anim Montage
	void PlayHitReactMontage(const FName& SectionName);
	void DirectionalHitReact(const FVector& ImpactPoint);

	void Die(const FVector& ImpactPoint);
	bool InTargetRange(AActor* Target , double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	void CheckCombatTarget();
	void CheckPatrolTarget();

	UPROPERTY(BlueprintReadOnly)
	EDeathPos DeathPos = EDeathPos::EDP_Alive;

private:
	//Attribute
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	//HUD (Health Bar)
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	//Animation Montage
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

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	//navigation

	UPROPERTY()
	AAIController* EnemyController;
	//current patrol target
	UPROPERTY(EditInstanceOnly , Category= "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly , Category= "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	UPROPERTY(EditAnywhere , Category= "AI Navigation")
	float WaitMin = 5.f;
	UPROPERTY(EditAnywhere , Category= "AI Navigation")
	float WaitMax = 10.f;
};
