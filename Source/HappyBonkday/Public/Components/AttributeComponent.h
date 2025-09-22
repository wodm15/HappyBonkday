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
	void RegenStamina(float DeltaTime);

protected:
	virtual void BeginPlay() override;

private:
	//current health
	UPROPERTY(EditAnywhere , Category = "Actor Attribute")
	float Health;

	UPROPERTY(EditAnywhere , Category = "Actor Attribute")
	float MaxHealth;

	//current stamina
	UPROPERTY(EditAnywhere , Category = "Actor Attribute")
	float Stamina;

	UPROPERTY(EditAnywhere , Category = "Actor Attribute")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attribute")
	int32 Gold;
	UPROPERTY(EditAnywhere , Category = "Actor Attribute")
	int32 Souls;

	UPROPERTY(EditAnywhere , Category = "Actor Attribute")
	float DodgeCost = 15.f;
	UPROPERTY(EditAnywhere , Category = "Actor Attribute")
	float CrounchCost = 10.f;

	UPROPERTY(EditAnywhere , Category = "Actor Attribute")
	float StaminaRegenRate = 8.f;

public:
	void ReceiveDamage(float Damage);
	void UseStamina(float StaminaCost);
	float GetHealthPercent();
	float GetStaminaPercent();
	bool IsAlive();
	void AddSouls(int32 NumberOfSouls);
	void AddGold(int32 AmountOfGold);

	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetCrounchCost() const { return CrounchCost; }
	FORCEINLINE float GetStamina() const { return Stamina; }
};
