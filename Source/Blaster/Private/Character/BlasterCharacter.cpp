#include "Character/BlasterCharacter.h"

#include "GameFramework/SpringArmComponent.h"

#include "Camera/CameraComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "InputMappingContext.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"

#include "Net/UnrealNetwork.h"

#include "Weapon/WeaponBase.h"

#include "BlasterComponents/CombatComponent.h"


ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoomComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoomComponent"));
	CameraBoomComponent->SetupAttachment(GetMesh());
	CameraBoomComponent->TargetArmLength = 600.f;
	CameraBoomComponent->bUsePawnControlRotation = true;

	FollowCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCameraComponent"));
	FollowCameraComponent->SetupAttachment(CameraBoomComponent, USpringArmComponent::SocketName);
	FollowCameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidgetComponent"));
	OverheadWidgetComponent->SetupAttachment(RootComponent);

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_AutonomousOnly);

}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (IsValid(CombatComponent))
	{
		CombatComponent->Init(this);
	}

}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffseet(DeltaTime);
}

void ABlasterCharacter::AimOffseet(float DeltaTime)
{
	if (IsValid(CombatComponent) && !IsValid(CombatComponent->EquippedWeapon))
	{
		return;
	}

	FVector velocity = GetVelocity();
	velocity.Z = 0.f;

	float speed = velocity.Size();

	bool bisInAir = GetCharacterMovement()->IsFalling();

	// standing still, not jumping
	if (speed == 0.f && !bisInAir)
	{
		FRotator currentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator deltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(currentAimRotation, StartingAimRotation);

		AO_Yaw = deltaAimRotation.Yaw;

		bUseControllerRotationYaw = false;
	}

	// running or jumping
	if (speed > 0.f || bisInAir)
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);

		AO_Yaw = 0.f;
	 	
		bUseControllerRotationYaw = true;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;

	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		FVector2D inRange(270.f, 360.f);
		FVector2D outRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(inRange, outRange, AO_Pitch);
	}
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerController = Cast<APlayerController>(GetController());

	UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	subsystem->ClearAllMappings();
	subsystem->AddMappingContext(InputMapping, 0);

	UEnhancedInputComponent* PlayerInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	PlayerInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::OnLook);

	PlayerInput->BindAction(GamepadLookAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::OnLookGamepad);

	PlayerInput->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::OnMove);

	PlayerInput->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABlasterCharacter::OnCrouch);

	PlayerInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ABlasterCharacter::OnJump);

	PlayerInput->BindAction(InteractAction, ETriggerEvent::Started, this, &ABlasterCharacter::OnInteract);

	PlayerInput->BindAction(PrimaryAction, ETriggerEvent::Started, this, &ABlasterCharacter::OnPrimaryActionStart);
	PlayerInput->BindAction(PrimaryAction, ETriggerEvent::Completed, this, &ABlasterCharacter::OnPrimaryActionEnd);

	PlayerInput->BindAction(SecondaryAction, ETriggerEvent::Started, this, &ABlasterCharacter::OnSecondaryActionStart);
	PlayerInput->BindAction(SecondaryAction, ETriggerEvent::Completed, this, &ABlasterCharacter::OnSecondaryActionEnd);
}

void ABlasterCharacter::OnLook(const FInputActionValue& Axis)
{
	FVector axisVector = Axis.Get<FVector>();
	AddControllerYawInput(axisVector.X);
	AddControllerPitchInput(axisVector.Y);
}

void ABlasterCharacter::OnLookGamepad(const FInputActionValue& Axis)
{
	if (GetWorld())
	{
		FVector axisVector = Axis.Get<FVector>() * UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		AddControllerYawInput(axisVector.X);
		AddControllerPitchInput(axisVector.Y);
	}
}

void ABlasterCharacter::OnMove(const FInputActionValue& Axis)
{
	if (!IsValid(Controller) ||
		!IsValid(GetWorld()))
	{
		return;
	}

	TargetInput = Axis.Get<FVector2D>().GetSafeNormal();

	const FRotator controlRotation = Controller->GetControlRotation();
	const FRotator yawRotation(0.f, controlRotation.Yaw, 0.f);

	const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(
		forwardDirection * TargetInput.Y +
		rightDirection * TargetInput.X,
		1.f);
}

void ABlasterCharacter::OnJump()
{
	Jump();
}

void ABlasterCharacter::OnCrouch()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void ABlasterCharacter::OnInteract()
{
	if (IsValid(CombatComponent))
	{
		if (HasAuthority())
		{
			CombatComponent->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			OnServerInteract();
		}
	}
}

void ABlasterCharacter::OnPrimaryActionStart()
{
}

void ABlasterCharacter::OnPrimaryActionEnd()
{
}

void ABlasterCharacter::OnSecondaryActionStart()
{
	if (IsValid(CombatComponent))
	{
		CombatComponent->SetAiming(true);
	}
}

void ABlasterCharacter::OnSecondaryActionEnd()
{
	if (IsValid(CombatComponent))
	{
		CombatComponent->SetAiming(false);
	}
}

void ABlasterCharacter::OnServerInteract_Implementation()
{
	if (IsValid(CombatComponent))
	{
		CombatComponent->EquipWeapon(OverlappingWeapon);
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeaponBase* Weapon)
{
	if (IsValid(OverlappingWeapon))
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = Weapon;

	if (IsLocallyControlled())
	{
		if (IsValid(OverlappingWeapon))
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeaponBase* LastWeapon)
{
	if (IsValid(OverlappingWeapon))
	{
		OverlappingWeapon->ShowPickupWidget(true);
		return;
	}
	if (IsValid(LastWeapon))
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return (IsValid(CombatComponent) && IsValid(CombatComponent->EquippedWeapon));
}

bool ABlasterCharacter::IsAiming()
{
	return (IsValid(CombatComponent) && CombatComponent->bIsAiming);
}
