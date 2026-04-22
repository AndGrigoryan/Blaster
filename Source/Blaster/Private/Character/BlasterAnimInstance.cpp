#include "Character/BlasterAnimInstance.h"

#include "Character/BlasterCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());

}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!IsValid(BlasterCharacter))
	{
		return;
	}

	FVector velocity = BlasterCharacter->GetVelocity();
	velocity.Z = 0.f;

	Speed = velocity.Size();

	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;

	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();

	bIsCrouched = BlasterCharacter->bIsCrouched;

	bIsAiming = BlasterCharacter->IsAiming();

	// Offset Yaw for Strafing
	FRotator aimRotation = BlasterCharacter->GetBaseAimRotation();
	FRotator movementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	FRotator deltaRot = UKismetMathLibrary::NormalizedDeltaRotator(movementRotation, aimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, deltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlasterCharacter->GetActorRotation();
	const FRotator delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float target = delta.Yaw / DeltaTime;
	const float interp = FMath::FInterpTo(Lean, target, DeltaTime, 6.f);
	Lean = FMath::Clamp(interp, -90.f, 90.f);
}
