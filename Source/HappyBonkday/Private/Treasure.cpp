// JY Sun All Rights Reserved


#include "Treasure.h"
#include "Character/BasicCharacter.h"
#include "Kismet/GamePlayStatics.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //Super::OnSphereOverlap();
	ABasicCharacter* BasicCharacter = Cast<ABasicCharacter>(OtherActor);
	if(BasicCharacter)
	{
		if(PickupSound)
        {
            UGameplayStatics::PlaySoundAtLocation(
                this,
                PickupSound,
                GetActorLocation()
            );
        }
        Destroy();
	}
}
