// JY Sun All Rights Reserved


#include "Character/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Weapon/Weapon.h"
#include "Components/AttributeComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
    Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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

void ABaseCharacter::PlayAttackMontage()
{

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
