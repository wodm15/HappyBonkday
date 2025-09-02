// JY Sun All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;

UCLASS()
class HAPPYBONKDAY_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();

	void Equip(USceneComponent* Inparent , FName InSocketName , AActor* NewOwner , APawn* NewInstigator);
	void AttachMeshToSocket(USceneComponent* Inparent , const FName& InSocketName);

	TArray<AActor*> IgnoreActors;
	
protected:
	virtual void BeginPlay() override;

	void PlayEquipSound();
	void DisableSphereCollision();
	void DeactivateEmbers();

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

	void ExecuteGetHit(FHitResult& BoxHit);

private:
	
	void BoxTrace(FHitResult& BoxHit);
	UPROPERTY(EditAnywhere , Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f);
	UPROPERTY(EditAnywhere , Category = "Weapon Properties")
	bool bShowBoxDebug = false;

	UPROPERTY(EditAnywhere , Category = "Weapon Properties")
	USoundBase* EquipSound;

	UPROPERTY(VisibleAnywhere , Category = "Weapon Properies")
	UBoxComponent* WeaponBox;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;
	UPROPERTY(EditAnywhere , Category = "Weapon Properies")
	float Damage = 20.f;

public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox;}
};
