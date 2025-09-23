// JY Sun All Rights Reserved

#pragma once

UENUM(BlueprintType)
enum class EStanceState : uint8
{
    ESS_Standing   UMETA(DisplayName = "Standing"),
    ESS_Crouching  UMETA(DisplayName = "Crouching")
};

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_HitReaction UMETA(DisplayName = "HitReaction"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_EquippingWeapon UMETA(DisplayName = "EquippingWeapon"),
	EAS_Dodge UMETA(DisplayName = "Dodge"),
	EAS_Crounching UMETA(DisplayName = "Crounching"),
	EAS_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
    MD_Idle     UMETA(DisplayName = "Idle"),
    MD_Forward  UMETA(DisplayName = "Forward"),
    MD_Backward UMETA(DisplayName = "Backward"),
    MD_Left     UMETA(DisplayName = "Left"),
    MD_Right    UMETA(DisplayName = "Right")
};

UENUM(BlueprintType)
enum class EDeathPos : uint8
{
	EDP_DeathLeft UMETA(DisplayName = "DeathLeft"),
	EDP_DeathRight UMETA(DisplayName = "DeathRight"),
	EDP_DeathBack UMETA(DisplayName = "DeathBack"),
	EDP_DeathFront UMETA(DisplayName = "DeathFront")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_NoState UMETA(DisplayName = "NoState"),
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged")
	
};