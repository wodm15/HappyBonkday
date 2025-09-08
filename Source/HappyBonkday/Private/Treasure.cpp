// JY Sun All Rights Reserved


#include "Treasure.h"
#include "Interfaces/PickupInterface.h"
#include "Kismet/GamePlayStatics.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if(PickupInterface)
	{
		PickupInterface->AddGold(this);

		SpawnPickupSound();
        Destroy();
	}

}
