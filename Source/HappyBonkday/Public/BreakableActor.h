// JY Sun All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "Treasure.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class HAPPYBONKDAY_API ABreakableActor : public AActor ,  public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hiiter) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere , BlueprintReadWrite)
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(VisibleAnywhere , BlueprintReadWrite)
	class UCapsuleComponent* Capsule;

private:
	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	TArray<TSubclassOf<class ATreasure>> TreasureClasses;

	bool bBroken = false;

};
