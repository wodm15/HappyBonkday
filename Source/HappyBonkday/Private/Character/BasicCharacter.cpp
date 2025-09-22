// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BasicCharacter.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"   
#include "Camera/CameraComponent.h"   


#include "Item.h"
#include "Weapon/Weapon.h"

#include "HUD/BasicHUD.h"
#include "HUD/BasicOverlay.h"
#include "Components/AttributeComponent.h"
#include "Soul.h"
#include "Treasure.h"

ABasicCharacter::ABasicCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f , 400.f , 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility , ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic , ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

}


void ABasicCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		InitializePlayerInput(PlayerController);
		InitializeBasicOverlay(PlayerController);
	}

	Tags.Add(FName("Player"));

	
}


void ABasicCharacter::Die_Implementation(const FVector& ImpactPoint)
{
	Super::Die_Implementation(ImpactPoint);

	ActionState = EActionState::EAS_Dead;

	DisableCapsule();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->bOrientRotationToMovement = false;
    SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
}


void ABasicCharacter::InitializePlayerInput(APlayerController* PlayerController)
{
	if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(SlashContext, 0);
		}
	}
	
	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = false; 
}

void ABasicCharacter::InitializeBasicOverlay(APlayerController* PlayerController)
{
	ABasicHUD* BasicHUD = Cast<ABasicHUD>(PlayerController->GetHUD());
	if(BasicHUD)
	{
		BasicOverlay = BasicHUD->GetBasicOverlay();
		if(BasicOverlay && Attributes)
		{
			BasicOverlay->SetHealthProgressBar(Attributes->GetHealthPercent());
			BasicOverlay->SetStaminaProgressBar(1.f);
			BasicOverlay->SetGold(0);
			BasicOverlay->SetSouls(0);
		}
	}
}


void ABasicCharacter::Tick(float DeltaTime)
{
	if(Attributes && BasicOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		BasicOverlay->SetStaminaProgressBar(Attributes->GetStaminaPercent());
	}
}

void ABasicCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction , ETriggerEvent::Triggered , this , &ABasicCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction , ETriggerEvent::Triggered , this , &ABasicCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction , ETriggerEvent::Triggered , this , &ABasicCharacter::Jump);
		EnhancedInputComponent->BindAction(EKeyPressedAction , ETriggerEvent::Triggered , this , &ABasicCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction , ETriggerEvent::Triggered , this , &ABasicCharacter::Attack);
		EnhancedInputComponent->BindAction(DodgeAction , ETriggerEvent::Triggered , this , &ABasicCharacter::Dodge);
		EnhancedInputComponent->BindAction(CrounchAction , ETriggerEvent::Triggered , this , &ABasicCharacter::Crounch);
	}

}
void ABasicCharacter::Jump()
{
	if(IsOccupiedOrCrounching()) return;
		
	Super::Jump();
	
}

bool ABasicCharacter::IsOccupied()
{
	return (ActionState != EActionState::EAS_Unoccupied && ActionState != EActionState::EAS_Crounch);
}

bool ABasicCharacter::IsOccupiedOrCrounching()
{
	return (ActionState != EActionState::EAS_Unoccupied && ActionState != EActionState::EAS_Crounch);
}


float ABasicCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator , DamageCauser );

	SetHUDHealth();

	return DamageAmount;
}




void ABasicCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hiiter)
{
	Super::GetHit_Implementation(ImpactPoint , Hiiter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}

}

void ABasicCharacter::SetOverlappingItem(class AItem* Item)
{
	OverlappingItem = Item;
}

void ABasicCharacter::AddSouls(class ASoul* Soul)
{
	if(Attributes && BasicOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		BasicOverlay->SetSouls(Attributes->GetSouls());
	}
}


void ABasicCharacter::AddGold(class ATreasure* Treasure)
{
	if(Attributes && BasicOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		BasicOverlay->SetGold(Attributes->GetGold());
	}
}



void ABasicCharacter::Move(const FInputActionValue& Value)
{
	if(IsOccupiedOrCrounching()) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = GetController()->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ABasicCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if(GetController())
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(-LookAxisValue.Y);
	}
}

void ABasicCharacter::EKeyPressed(const FInputActionValue& Value)
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if(OverlappingWeapon)
		{
			if (EquippedWeapon)
			{
				EquippedWeapon->Destroy();
			}
			EquipWeapon(OverlappingWeapon);
		}
	else
		{
		if(CanDisarm())
			{
				DisArm();
			}
		else if(CanArm())
			{
				Arm();
			}
		}

}


void ABasicCharacter::Dodge(const FInputActionValue& Value)
{	
	if(IsOccupied() || !HasEnoughStamina()) return;
	

	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
	if(Attributes && BasicOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		BasicOverlay->SetStaminaProgressBar(Attributes->GetStaminaPercent());
	}

}


void ABasicCharacter::Crounch(const FInputActionValue& Value)
{
	if(IsOccupiedOrCrounching() || !HasEnoughStamina()) return;

	if (ActionState == EActionState::EAS_Crounch)
	{
		PlayStandMontage();
		IsCrounching = false;
	}
	else
	{
		PlayCrounchMontage();
		IsCrounching = true;
	}

	if(Attributes && BasicOverlay)
	{
		Attributes->UseStamina(Attributes->GetCrounchCost());
		BasicOverlay->SetStaminaProgressBar(Attributes->GetStaminaPercent());
	}
}


bool ABasicCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

bool ABasicCharacter::CanDisarm()
{
    return ActionState == EActionState::EAS_Unoccupied && 
           CharacterState == ECharacterState::ECS_EquippedOneHandedWeapon &&
           EquippedWeapon;
}

bool ABasicCharacter::CanArm()
{
    return ActionState == EActionState::EAS_Unoccupied && 
           CharacterState == ECharacterState::ECS_Unequipped &&
           EquippedWeapon;
}

void ABasicCharacter::DisArm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}


void ABasicCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ABasicCharacter::Attack()
{
	Super::Attack();

	if(CanAttack())
	{
		ActionState = EActionState::EAS_Attacking;
		PlayAttackMontage();
	}
}

bool ABasicCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied && 
		CharacterState != ECharacterState::ECS_Unequipped;
}


void ABasicCharacter::AttachWeaponToBack()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}


void ABasicCharacter::AttachWeaponToHand()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("HandRightSocket"));
	}
}


void ABasicCharacter::EquipWeapon(AWeapon* Weapon)
{
		Weapon->Equip(GetMesh() , FName("HandRightSocket") , this , this);
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		OverlappingItem = nullptr;
		EquippedWeapon = Weapon;
}

void ABasicCharacter::PlayEquipMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}


void ABasicCharacter::AttackEnd()
{
	ActionState =  EActionState::EAS_Unoccupied;
}

void ABasicCharacter::DodgeEnd()
{
	ActionState =  EActionState::EAS_Unoccupied;
}

void ABasicCharacter::CrounchEnd()
{
	ActionState =  EActionState::EAS_Crounch;
}

void ABasicCharacter::StandEnd()
{
	ActionState =  EActionState::EAS_Unoccupied;
}

void ABasicCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ABasicCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ABasicCharacter::SetHUDHealth()
{
	if(BasicOverlay)
	{
		BasicOverlay->SetHealthProgressBar(Attributes->GetHealthPercent());
	}
}