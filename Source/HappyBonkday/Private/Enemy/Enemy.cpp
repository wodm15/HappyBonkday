// JY Sun All Rights Reserved


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GamePlayStatics.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
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
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera , ECollisionResponse::ECR_Ignore);

    Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

    HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
    HealthBarWidget->SetupAttachment(GetRootComponent());

    GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터 가속 방향으로 이동
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 1000.f;
    SightConfig->LoseSightRadius = 1200.f;
    SightConfig->PeripheralVisionAngleDegrees = 60.f;
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

    if(HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(false);
    }
    EnemyController = Cast<AAIController>(GetController());
    MoveToTarget(PatrolTarget);

    if(AIPerception)
    {
        AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemy::PawnSeen);
    }
}


void AEnemy::PawnSeen(AActor* SeenPawn, FAIStimulus Stimulus)
{
    if(!SeenPawn->ActorHasTag("Player")) return; // Player 태그 없으면 무시
    if(EnemyState ==  EEnemyState::EES_Chasing) return;

    if(Stimulus.WasSuccessfullySensed())
    {   
        GetWorld()->GetTimerManager().ClearTimer(PatrolTimer);
        GetCharacterMovement()->MaxWalkSpeed = 300.f;
        CombatTarget = SeenPawn;
        
        if(EnemyState != EEnemyState::EES_Attacking)
        {
            EnemyState = EEnemyState::EES_Chasing;
            MoveToTarget(CombatTarget);
        }
    }
}



void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
    if(!InTargetRange(CombatTarget , CombatRadius))
    {
        //outside combat radius , then lose interest
        CombatTarget = nullptr;
        if(HealthBarWidget)
        {
            HealthBarWidget->SetVisibility(false);
        }
        EnemyState = EEnemyState::EES_Patrolling;
        GetCharacterMovement()->MaxWalkSpeed = 125.f;
        MoveToTarget(PatrolTarget);
        UE_LOG(LogTemp, Display, TEXT("lose interest"));
    }
    else if(!InTargetRange(CombatTarget , AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
    {
        //outside Attack radius, then Chasing player
        EnemyState = EEnemyState::EES_Chasing;
        GetCharacterMovement()->MaxWalkSpeed = 300.f;
        MoveToTarget(CombatTarget);
         UE_LOG(LogTemp, Display, TEXT("chasing"));
    }
    else if(InTargetRange(CombatTarget , AttackRadius) && EnemyState != EEnemyState::EES_Attacking)
    {
        EnemyState = EEnemyState::EES_Attacking;
         UE_LOG(LogTemp, Display, TEXT("attack"));
        //todo: attack montage
    }

}

void AEnemy::CheckPatrolTarget()
{
    if(InTargetRange(PatrolTarget , PatrolRadius))
    {
        PatrolTarget = ChoosePatrolTarget();
        const float WaitTime = FMath::RandRange(WaitMin, WaitMax);
        GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
    }
}


// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AEnemy::MoveToTarget(AActor* Target)
{
    if(EnemyController == nullptr || Target == nullptr) return;

    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(Target);
    MoveRequest.SetAcceptanceRadius(15.f);
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


void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{

    if(HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(true);
    }
    if(Attributes && Attributes->IsAlive())
    {
        DirectionalHitReact(ImpactPoint);   
    }
    else
    {
        Die(ImpactPoint);
    }

	if(HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
    if(HitParticles && GetWorld())
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            HitParticles,
            ImpactPoint
        );
    }
}


void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
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

    FName Section = FName("FromBack");
    if (Theta >= -45.f && Theta < 45.f)
        Section = FName("FromFront");
    else if (Theta >= -135.f && Theta < 45.f)
        Section = FName("FromLeft");
    else if (Theta >= 45.f && Theta < 135.f)
        Section = FName("FromRight");

    PlayHitReactMontage(Section);

    // 디버그
    //UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Blue, 5.f);
    //UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f);
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    if(Attributes)
    {
        Attributes->ReceiveDamage(DamageAmount);

        if(HealthBarWidget)
        {
            HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
        }
        
        if (!Attributes->IsAlive())
        {
            Die(FVector(0,0,0));
        }

        CombatTarget = EventInstigator->GetPawn();

        //when hit ? enemy anger
        EnemyState = EEnemyState::EES_Chasing;
        GetCharacterMovement()->MaxWalkSpeed = 300.f;
        MoveToTarget(CombatTarget);
    }

    return DamageAmount;
}

void AEnemy::Die(const FVector& ImpactPoint)
{
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

    if(HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(false);
    }
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 죽으면 캐릭터 컨트롤러 뺏기(카메라 시점으로 이동하기 때문)
    GetCharacterMovement()->DisableMovement();
    GetCharacterMovement()->StopMovementImmediately();
    if (Controller) Controller->UnPossess();
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;
    
    SetLifeSpan(10.f);

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
