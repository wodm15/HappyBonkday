// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BasicCharacter.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "GameFramework/SpringArmComponent.h"   
#include "Camera/CameraComponent.h"   
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"

#include "Item.h"
#include "Weapon/Weapon.h"

ABasicCharacter::ABasicCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	SpringArm->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));

	
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

}


void ABasicCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->AddMappingContext(SlashContext, 0);
			}
		}
	}
	
}


void ABasicCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	}

}

void ABasicCharacter::Move(const FInputActionValue& Value)
{
	if(ActionState != EActionState::EAS_Unoccupied) return;

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
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void ABasicCharacter::EKeyPressed(const FInputActionValue& Value)
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
		if(OverlappingWeapon)
		{
			OverlappingWeapon->Equip(GetMesh() , FName("HandRightSocket") , this , this);
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			OverlappingItem = nullptr;
			EquippedWeapon = OverlappingWeapon;

		}
		else
		{
		if(CanDisarm())
		{
			PlayEquipMontage(FName("Unequip"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
		else if(CanArm())
		{
			PlayEquipMontage(FName("Equip"));
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
		}

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

void ABasicCharacter::Attack(const FInputActionValue& Value)
{
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


void ABasicCharacter::Disarm()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}


void ABasicCharacter::Arm()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("HandRightSocket"));
	}
}

void ABasicCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		const int32 Selection = FMath::RandRange(0,1);
		FName SelectionName = FName();

		switch(Selection)
		{
			case 0:
				SelectionName = FName("Attack1");
				break;

			case 1:
				SelectionName = FName("Attack2");
				break;

			default:
				break;

		}
		AnimInstance->Montage_JumpToSection(SelectionName, AttackMontage);
	}
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

void ABasicCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ABasicCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if(EquippedWeapon && EquippedWeapon->GetWeaponBox() )
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}
