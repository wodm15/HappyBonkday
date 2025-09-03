// JY Sun All Rights Reserved


#include "Character/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Weapon/Weapon.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GamePlayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
    Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera , ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
    if(IsAlive() && Hitter)
    {
        DirectionalHitReact(Hitter->GetActorLocation());   
    }
    else
    {
        Die(ImpactPoint);
    }

	PlayHitSound(ImpactPoint);
    SpawnHitParticles(ImpactPoint);
}


void ABaseCharacter::Attack()
{

}

void ABaseCharacter::AttackEnd()
{

}

bool ABaseCharacter::CanAttack()
{
	return true;
}


bool ABaseCharacter::IsAlive()
{
    return Attributes->IsAlive();
}


void ABaseCharacter::Die(const FVector& ImpactPoint)
{

}


void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}


void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
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


void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if(HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}


void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
    if(HitParticles && GetWorld())
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            HitParticles,
            ImpactPoint
        );
    }
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
    if(Attributes)
        {
            Attributes->ReceiveDamage(DamageAmount);
        }
            
}


void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage ,const FName& SectionName)
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if(AnimInstance && Montage)
    {
        AnimInstance->Montage_Play(Montage);
        AnimInstance->Montage_JumpToSection(SectionName, Montage);
    }
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage , const TArray<FName>& SectionNames)
{
    if(SectionNames.Num() <= 0) return -1;
    const int32 MaxSelectionIndex = SectionNames.Num()-1;
    const int32 Selection = FMath::RandRange(0,MaxSelectionIndex);
    PlayMontageSection(Montage , SectionNames[Selection]);
    return Selection;
}


int32 ABaseCharacter::PlayAttackMontage()
{
    return PlayRandomMontageSection(AttackMontage , AttackMontageSections);
}

int32 ABaseCharacter::PlayDeathMontage()
{
    return PlayRandomMontageSection(DeathMontage , DeathMontageSections);
}


void ABaseCharacter::StopAttackMontage()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if(AnimInstance)
    {
        AnimInstance->Montage_Stop(0.25f, AttackMontage);
    }
}


FVector ABaseCharacter::GetTranslationWarpTarget()
{
    if(CombatTarget == nullptr)
        return FVector();

    const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
    const FVector Location = GetActorLocation();
    FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal(); 
    TargetToMe *= WarpTargetDistance;

    return CombatTargetLocation + TargetToMe;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
    if(CombatTarget)
    {
        return CombatTarget->GetActorLocation();
    }
    return FVector();
}


void ABaseCharacter::DisableCapsule()
{
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if(EquippedWeapon && EquippedWeapon->GetWeaponBox() )
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}
