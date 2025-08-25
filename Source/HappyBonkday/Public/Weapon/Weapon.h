// JY Sun All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class HAPPYBONKDAY_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();

	void Equip(USceneComponent* Inparent , FName InSocketName , AActor* NewOwner , APawn* NewInstigator);
	void AttachMeshToSocket(USceneComponent* Inparent , const FName& InSocketName);

protected:
	virtual void BeginPlay() override;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	virtual void OnSphereOverLapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:
	UPROPERTY(EditAnywhere , Category = "Weapon Properties")
	USoundBase* EquipSound;
};
