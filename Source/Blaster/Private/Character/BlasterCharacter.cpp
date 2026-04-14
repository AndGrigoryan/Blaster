#include "Character/BlasterCharacter.h"

#include "GameFramework/SpringArmComponent.h"

#include "Camera/CameraComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "InputMappingContext.h"

#include "Kismet/GameplayStatics.h"


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


}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	if (!IsValid(Controller) &&
		!IsValid(GetWorld()))
	{
		return;
	}

	TargetInput = Axis.Get<FVector2D>().GetSafeNormal();

	AddMovementInput(
		GetActorForwardVector() * TargetInput.Y +
		GetActorRightVector() * TargetInput.X,
		1.f);
}

void ABlasterCharacter::OnJump()
{
	Jump();
}

void ABlasterCharacter::OnCrouch()
{
}

