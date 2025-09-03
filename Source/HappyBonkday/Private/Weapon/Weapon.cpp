// JY Sun All Rights Reserved


#include "Weapon/Weapon.h"
#include "Character/BasicCharacter.h"
#include "Kismet/GamePlayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
    WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
    WeaponBox->SetupAttachment(GetRootComponent());
    WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn , ECollisionResponse::ECR_Ignore);

    BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
    BoxTraceStart->SetupAttachment(GetRootComponent());
    BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
    BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
    
    WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::Equip(USceneComponent* Inparent , FName InSocketName , AActor* NewOwner , APawn* NewInstigator)
{
    ItemState = EItemState::EIS_Equipped;
    SetOwner(NewOwner);
    SetInstigator(NewInstigator);
    AttachMeshToSocket(Inparent, InSocketName);
    DisableSphereCollision();
    PlayEquipSound();
    DeactivateEmbers();
}

void AWeapon::AttachMeshToSocket(USceneComponent* Inparent , const FName& InSocketName)
{
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget , true);
    ItemMesh->AttachToComponent(Inparent, TransformRules, InSocketName);
}

void AWeapon::PlayEquipSound()
{
    if(EquipSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            EquipSound,
            GetActorLocation()
        );
    }
}

void AWeapon::DisableSphereCollision()
{
    if(Sphere)
    {
        Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

void AWeapon::DeactivateEmbers()
{
    if(EmbersEffect)
    {
        EmbersEffect->Deactivate();
    }
}


void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if(GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy")))
    {
        return;
    }

    FHitResult BoxHit;
    BoxTrace(BoxHit);

    if(BoxHit.GetActor())
    {
        if(GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy")))
        {
            return;
        }

        ExecuteGetHit(BoxHit);
        CreateFields(BoxHit.ImpactPoint);

        //Damage
        UGameplayStatics::ApplyDamage(
            BoxHit.GetActor(),
            Damage,
            GetInstigator()->GetController(),
            this,
            UDamageType::StaticClass()
        );
    }
}


void AWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
    IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
    if(HitInterface)
        {
            //HitInterface->GetHit(BoxHit.ImpactPoint);
            //BP , c++  both override
            HitInterface->Execute_GetHit(BoxHit.GetActor() , BoxHit.ImpactPoint , GetOwner());
        }
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
    const FVector Start = BoxTraceStart->GetComponentLocation();
    const FVector End = BoxTraceEnd->GetComponentLocation();

    TArray<AActor*> ActorsToIgnore;
    //ActorsToIgnore.Add(this);
    ActorsToIgnore.Add(GetOwner());

    for(AActor* Actor : IgnoreActors)
    {
        ActorsToIgnore.AddUnique(Actor);
    }


    UKismetSystemLibrary::BoxTraceSingle(
        this,
        Start,
        End,
        BoxTraceExtent,
        BoxTraceStart->GetComponentRotation(),
        ETraceTypeQuery::TraceTypeQuery1,
        false,
        ActorsToIgnore,
        bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
        BoxHit,
        true
    );
    IgnoreActors.AddUnique(BoxHit.GetActor());
}
