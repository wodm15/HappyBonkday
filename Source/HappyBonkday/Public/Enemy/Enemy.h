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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
protected:
	virtual void BeginPlay() override;



	virtual void Die(const FVector& ImpactPoint) override;
	bool InTargetRange(AActor* Target , double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(AActor* SeenPawn, FAIStimulus Stimulus);

	void CheckCombatTarget();
	void CheckPatrolTarget();

	UPROPERTY(BlueprintReadOnly)
	EDeathPos DeathPos = EDeathPos::EDP_Alive;

private:
//component

	//HUD (Health Bar)
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* AIPerception;
//


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

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
};
