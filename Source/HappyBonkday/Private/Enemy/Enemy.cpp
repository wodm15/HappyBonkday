// JY Sun All Rights Reserved


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Weapon/Weapon.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"

AEnemy::AEnemy()
{

	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility , ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera , ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	
    HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
    HealthBarWidget->SetupAttachment(GetRootComponent());

    GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터 가속 방향으로 이동
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 1500.f;
    SightConfig->LoseSightRadius = 1800.f;
    SightConfig->PeripheralVisionAngleDegrees = 120.f;
    SightConfig->SetMaxAge(5.f);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    AIPerception->ConfigureSense(*SightConfig);
    AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
    if(AIPerception) AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemy::PawnSeen);
    InitializeEnemy();

    Tags.Add(FName("Enemy"));
}

void AEnemy::InitializeEnemy()
{
    EnemyController = Cast<AAIController>(GetController());
    HideHealthBar();
    MoveToTarget(PatrolTarget);
    SpawnDefaultWeapon();
}

void AEnemy::SpawnDefaultWeapon()
{
    UWorld* World = GetWorld();
    if(World && WeaponClass)
    {
        AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
        DefaultWeapon->Equip(GetMesh() , FName("RightHandSocket") , this, this);
        EquippedWeapon = DefaultWeapon;
    }
}

void AEnemy::Attack()
{
    EnemyState = EEnemyState::EES_Engaged;
    Super::Attack();
    PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
    bool bCanAttack = IsInsideAttackRadius() && !IsAttacking() && !IsEngaged() && !IsDead();
    return bCanAttack;
}

void AEnemy::HandleDamage(float DamageAmount)
{
    Super::HandleDamage(DamageAmount);

    if(Attributes && HealthBarWidget)
    {
        HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
    }
}

void AEnemy::AttackEnd()
{
    EnemyState = EEnemyState::EES_NoState;
    CheckCombatTarget();
    
    if (EquippedWeapon)
    {
        EquippedWeapon->ClearIgnoredActors();
    }
}


void AEnemy::PawnSeen(AActor* SeenPawn, FAIStimulus Stimulus)
{
    if (!SeenPawn || !Stimulus.WasSuccessfullySensed()) return;

    if (EnemyState == EEnemyState::EES_Dead) return;

    if (!SeenPawn->ActorHasTag("Player")) return;

    CombatTarget = SeenPawn;
    CleanPatrolTimer();

    if (IsInsideAttackRadius())
    {
        StartAttackTimer(); 
    }
    else
    {
        ChaseTarget();     
    }
}




void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if(IsDead()) return;
    if(EnemyState > EEnemyState::EES_Patrolling)
    {
        CheckCombatTarget();
    }
    else
    {
        CheckPatrolTarget();
    }
}

void AEnemy::CheckCombatTarget()
{

    if(IsOutsideCombatRadius())
    {
        ClearAttackTimer();
        LostInterest();
        if(!IsEngaged())
        {
            StartPatrolling();
        }
    }

    else if(IsOutsideAttackRadius() && !IsChasing())
    {
        ClearAttackTimer();
        if(!IsEngaged()) ChaseTarget();
        
    }
    else if(CanAttack())
    {
        StartAttackTimer();
    }

}

void AEnemy::CheckPatrolTarget()
{
    if(InTargetRange(PatrolTarget , PatrolRadius))
    {
        PatrolTarget = ChoosePatrolTarget();
        const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
        GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
    }
}


/* Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

} 
*/


void AEnemy::MoveToTarget(AActor* Target)
{
    if(EnemyController == nullptr || Target == nullptr) return;

    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(Target);
    MoveRequest.SetAcceptanceRadius(50.f);
    EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
    TArray<AActor*> ValidTargets;
    for(AActor* Target : PatrolTargets)
    {
        if(Target != PatrolTarget)
        {
            ValidTargets.AddUnique(Target);
        }
    }
    const int32 NumPatrolTargets = ValidTargets.Num();
    if(NumPatrolTargets > 0)
    {
        const int32 TargetSelection = FMath::RandRange(0 , NumPatrolTargets-1);
        return ValidTargets[TargetSelection];
    }

    return nullptr;
}


void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{

    Super::GetHit_Implementation(ImpactPoint , Hitter);
    if (!IsDead()) ShowHealthBar();
    CleanPatrolTimer();
    ClearAttackTimer();

    SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
    StopAttackMontage();
}



float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    HandleDamage(DamageAmount);
    CombatTarget = EventInstigator->GetPawn();
   
    if (!Attributes->IsAlive())
    {
        Die(FVector::ZeroVector);
        return DamageAmount;
    }
	if (IsInsideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (IsOutsideAttackRadius())
	{
		ChaseTarget();
	}

    return DamageAmount;
}

void AEnemy::Die(const FVector& ImpactPoint)
{
    EnemyState = EEnemyState::EES_Dead;
    ClearAttackTimer();

    const FVector Forward = GetActorForwardVector();
    const FVector ToHit = (ImpactPoint - GetActorLocation()).GetSafeNormal();

    float CosTheta = FVector::DotProduct(Forward, ToHit);
    float Theta = FMath::Acos(CosTheta);
    Theta = FMath::RadiansToDegrees(Theta);

    FVector Cross = FVector::CrossProduct(Forward, ToHit);
    if (Cross.Z < 0)
    {
        Theta *= -1.f; // 왼쪽이면 음수(언리얼은 왼손 법칙)
    }

    FName SelectionName = FName("DeathBack");
    DeathPos = EDeathPos::EDP_DeathBack;

    if (Theta >= -45.f && Theta < 45.f)
    {
        SelectionName = FName("DeathFront");
        DeathPos = EDeathPos::EDP_DeathFront;
    }
    else if (Theta >= -135.f && Theta < 45.f)
    {
        SelectionName = FName("DeathLeft");
        DeathPos = EDeathPos::EDP_DeathLeft;
    }

    else if (Theta >= 45.f && Theta < 135.f)
    {
        SelectionName = FName("DeathRight");
        DeathPos = EDeathPos::EDP_DeathRight;
    }
        
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && DeathMontage)
	{
        AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(SelectionName, DeathMontage);
	}

    HideHealthBar();
    DisableCapsule();
    SetLifeSpan(DeathLifeSpan);
    GetCharacterMovement()->bOrientRotationToMovement = false;
    SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

}


bool AEnemy::InTargetRange(AActor* Target , double Radius)
{
    if(Target == nullptr) return false;
    const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
    return DistanceToTarget <= Radius;
}


void AEnemy::PatrolTimerFinished()
{
    MoveToTarget(PatrolTarget);
}


void AEnemy::Destroyed()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->Destroy();
    }
}


void AEnemy::HideHealthBar()
{
    if(HealthBarWidget)
        {
            HealthBarWidget->SetVisibility(false);
        }
}

void AEnemy::ShowHealthBar()
{
    if(HealthBarWidget)
        {
            HealthBarWidget->SetVisibility(true);
        }
}

void AEnemy::LostInterest()
{
    CombatTarget = nullptr;
    HideHealthBar();
}

void AEnemy::StartPatrolling()
{
    EnemyState = EEnemyState::EES_Patrolling;
    GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
    MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
    EnemyState = EEnemyState::EES_Chasing;
    GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
    MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutsideCombatRadius()
{
    return !InTargetRange(CombatTarget , CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
    return !InTargetRange(CombatTarget , AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
    return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing()
{
    return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
    return EnemyState == EEnemyState::EES_Attacking;
}

void AEnemy::StartAttackTimer()
{
    if(IsDead()) return;
    EnemyState = EEnemyState::EES_Attacking;
    const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
    GetWorldTimerManager().SetTimer(AttackTimer, this , &AEnemy::Attack, AttackTime);
}

bool AEnemy::IsDead()
{
    return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
    return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::CleanPatrolTimer()
{
    GetWorld()->GetTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::ClearAttackTimer()
{
    GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
}
