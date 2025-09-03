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

ABasicCharacter::ABasicCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
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

	Tags.Add(FName("Player"));

	
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

void ABasicCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hiiter)
{
	Super::GetHit_Implementation(ImpactPoint , Hiiter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	ActionState = EActionState::EAS_HitReaction;
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

void ABasicCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ABasicCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

