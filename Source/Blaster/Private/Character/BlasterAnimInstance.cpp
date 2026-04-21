#include "Character/BlasterAnimInstance.h"

#include "Character/BlasterCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"


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

}
