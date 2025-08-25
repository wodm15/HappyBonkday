// JY Sun All Rights Reserved


#include "Weapon/Weapon.h"
#include "Character/BasicCharacter.h"
#include "Kismet/GamePlayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{

}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
    
}

void AWeapon::Equip(USceneComponent* Inparent , FName InSocketName , AActor* NewOwner , APawn* NewInstigator)
{
    SetOwner(NewOwner);
    SetInstigator(NewInstigator);
    AttachMeshToSocket(Inparent, InSocketName);
    //ItemState = EItemState::EIS_Equipped;

    if(EquipSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            EquipSound,
            GetActorLocation()
        );
    }
    if(Sphere)
    {
        Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    if(EmbersEffect)
    {
        EmbersEffect->Deactivate();
    }
}

void AWeapon::AttachMeshToSocket(USceneComponent* Inparent , const FName& InSocketName)
{
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget , true);
    ItemMesh->AttachToComponent(Inparent, TransformRules, InSocketName);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep,SweepResult);

    ABasicCharacter* BasicCharacter = Cast<ABasicCharacter>(OtherActor);
    // if(BasicCharacter)
    // {
    //     FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget , true);
    //     ItemMesh->AttachToComponent(BasicCharacter->GetMesh(), TransformRules, FName("HandRightSocket"));
    //     ItemMesh->SetRelativeScale3D(FVector(0.1f));
    // }
}

void AWeapon::OnSphereOverLapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    Super::OnSphereOverLapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}