// JY Sun All Rights Reserved


#include "Item.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/BasicCharacter.h"
#include "NiagaraComponent.h"
// Sets default values
AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(RootComponent);

	EmbersEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	EmbersEffect->SetupAttachment(RootComponent);

}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this , &AItem::OnSphereOverLapEnd);
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABasicCharacter* BasicCharacter = Cast<ABasicCharacter>(OtherActor);
	if(BasicCharacter)
	{
		BasicCharacter->SetOverlappingItem(this);
	}

}

void AItem::OnSphereOverLapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABasicCharacter* BasicCharacter = Cast<ABasicCharacter>(OtherActor);
	if(BasicCharacter)
	{
		BasicCharacter->SetOverlappingItem(nullptr);
	}
}