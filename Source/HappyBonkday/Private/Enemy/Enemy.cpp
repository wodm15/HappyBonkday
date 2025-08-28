// JY Sun All Rights Reserved


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GamePlayStatics.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"

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
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

    if(HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(false);
    }
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if(CombatTarget)
    {
        const double DistanceToTarget = (CombatTarget->GetActorLocation() - GetActorLocation()).Size();
        if(DistanceToTarget > CombatRadius)
        {
            CombatTarget = nullptr;
            if(HealthBarWidget)
            {
                HealthBarWidget->SetVisibility(false);
            }
        }
    }
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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
    SetLifeSpan(10.f);

}