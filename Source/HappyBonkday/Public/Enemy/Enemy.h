// JY Sun All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Character/CharacterTypes.h"

#include "Enemy.generated.h"

class UHealthBarComponent;
class AAIController;
class UAIPerceptionComponent;

UCLASS()
class HAPPYBONKDAY_API AEnemy : public ABaseCharacter 
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
protected:
	virtual void BeginPlay() override;



	virtual void Die(const FVector& ImpactPoint) override;
	bool InTargetRange(AActor* Target , double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	//virtual void PlayDeathMontage() override;

	UPROPERTY(EditAnywhere , Category = Combat)
	float DeathLifeSpan = 10.f;

	UFUNCTION()
	void PawnSeen(AActor* SeenPawn, FAIStimulus Stimulus);

	void CheckCombatTarget();
	void CheckPatrolTarget();

	UPROPERTY(BlueprintReadOnly)
	EDeathPos DeathPos;
	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:
//component

	//HUD (Health Bar)
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* AIPerception;
//
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;


	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;
	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

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

	//ai behavior
	void HideHealthBar();
	void ShowHealthBar();
	void LostInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void CleanPatrolTimer();

	//Combat
	void StartAttackTimer();
	void ClearAttackTimer();

	FTimerHandle AttackTimer;
	
	UPROPERTY(EditAnywhere , Category = "Combat")
	float AttackMin = 0.5f;
	UPROPERTY(EditAnywhere , Category = "Combat")
	float AttackMax = 1.f;


	UPROPERTY(EditAnywhere , Category="Combat")
	float PatrollingSpeed = 125.f;
	UPROPERTY(EditAnywhere , Category="Combat")
	float ChasingSpeed = 300.f;
};
