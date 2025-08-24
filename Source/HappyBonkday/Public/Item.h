// JY Sun All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;
class UStaticMeshComponent;

enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class HAPPYBONKDAY_API AItem : public AActor
{
	GENERATED_BODY()

public:	
	AItem();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere) 
	class UNiagaraComponent* EmbersEffect;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void OnSphereOverLapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item", meta=(AllowPrivateAccess="true"))
	USphereComponent* Sphere;

};
