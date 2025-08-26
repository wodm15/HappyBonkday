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

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    const FVector Start = BoxTraceStart->GetComponentLocation();
    const FVector End = BoxTraceEnd->GetComponentLocation();

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);

    //for(AActor* Actor : IgnoreActors)
    //{
       //ActorsToIgnore.AddUnique(Actor);
    //}
    FHitResult BoxHit;

    UKismetSystemLibrary::BoxTraceSingle(
        this,
        Start,
        End,
        FVector(5.f, 5.f , 5.f),
        BoxTraceStart->GetComponentRotation(),
        ETraceTypeQuery::TraceTypeQuery1,
        false,
        ActorsToIgnore,
        EDrawDebugTrace::ForDuration,
        BoxHit,
        true
    );
    if(BoxHit.GetActor())
    {
        IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
        if(HitInterface)
        {
            HitInterface->GetHit(BoxHit.ImpactPoint);
        }

    }
}
