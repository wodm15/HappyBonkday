// JY Sun All Rights Reserved


#include "BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/CapsuleComponent.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

    GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
    SetRootComponent(GeometryCollection);

    GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    
    Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
    Capsule->SetupAttachment(GetRootComponent());
    Capsule->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
    Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint , AActor* Hiiter)
{
    if(bBroken) return;
    bBroken = true;
    UWorld* World = GetWorld();
    if(World && TreasureClasses.Num() > 0)
    {
        FVector Location = GetActorLocation();
        Location.Z += 75.f;

        int32 Section = FMath::RandRange(0, TreasureClasses.Num()-1); 
        World->SpawnActor<ATreasure>(TreasureClasses[Section] , Location, GetActorRotation());
    }

}
