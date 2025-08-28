// JY Sun All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HAPPYBONKDAY_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


protected:
	virtual void BeginPlay() override;

private:
	//current health
	UPROPERTY(EditAnywhere , Category = "Actor Attribute")
	float Health;

	UPROPERTY(EditAnywhere , Category = "Actor Attribute")
	float MaxHealth;

public:
	void ReceiveDamage(float Damage);
	float GetHealthPercent();
	bool IsAlive();
};
