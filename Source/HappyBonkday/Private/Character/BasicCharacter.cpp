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

ABasicCharacter::ABasicCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

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
	//if(ActionState != EActionState::EAS_Unoccupied) return;

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
}

void ABasicCharacter::Attack(const FInputActionValue& Value)
{
}
