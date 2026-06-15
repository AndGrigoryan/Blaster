#include "Character/BlasterAnimInstance.h"

#include "Character/BlasterCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Weapon/WeaponBase.h"


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

	bIsAccelerating = Speed > 3.f && !bIsInAir;

	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();

	EquippedWeapon = BlasterCharacter->GetEquippedWeapon();

	bIsCrouched = BlasterCharacter->bIsCrouched;

	bIsAiming = BlasterCharacter->IsAiming();

	//TurningInPlace = BlasterCharacter->GetTurningInPlace();

	//bIsTurningInPlace = TurningInPlace != ETurningInPlace::ETIP_NotTurning;

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

	AO_Yaw = BlasterCharacter->GetAO_Yaw();

	AO_Pitch = BlasterCharacter->GetAO_Pitch();

	if (bWeaponEquipped && 
		IsValid(EquippedWeapon) && 
		IsValid(EquippedWeapon->GetWeaponMesh()) && 
		IsValid(BlasterCharacter->GetMesh()))
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);

		FVector outPosition;
		FRotator outRotation;

		BlasterCharacter->GetMesh()->TransformToBoneSpace
		(
			FName("hand_r"), 
			LeftHandTransform.GetLocation(), 
			FRotator::ZeroRotator, 
			outPosition, 
			outRotation
		);

		LeftHandTransform.SetLocation(outPosition);
		LeftHandTransform.SetRotation(FQuat(outRotation)); 
	}
}
