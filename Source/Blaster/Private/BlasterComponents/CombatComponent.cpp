#include "BlasterComponents/CombatComponent.h"

#include "Character/BlasterCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Weapon/WeaponBase.h"

#include "Engine/SKeletalMeshSocket.h"

#include "Components/SphereComponent.h"

#include "Net/UnrealNetwork.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	BaseWalkSpeed = 600.f;

	AimWalkSpeed = 450.f;

}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(OwnerCharacter))
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}

}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);

	DOREPLIFETIME(UCombatComponent, bIsAiming);

}

void UCombatComponent::Init(ABlasterCharacter* InOwnerCharacter)
{
	OwnerCharacter = InOwnerCharacter;
}

void UCombatComponent::EquipWeapon(AWeaponBase* InWeaponToEquip)
{
	if (!IsValid(OwnerCharacter) ||
		!IsValid(InWeaponToEquip))
	{
		return;
	}

	EquippedWeapon = InWeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	const USkeletalMeshSocket* rightHandSocket = OwnerCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (IsValid(rightHandSocket))
	{
		rightHandSocket->AttachActor(EquippedWeapon, OwnerCharacter->GetMesh());
	}

	EquippedWeapon->SetOwner(OwnerCharacter);

	OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	OwnerCharacter->bUseControllerRotationYaw = true;
}

void UCombatComponent::SetAiming(bool bInIsAiming)
{
	bIsAiming = bInIsAiming;
	ServerSetAiming(bIsAiming);
	if (IsValid(OwnerCharacter))
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}

}

void UCombatComponent::ServerSetAiming_Implementation(bool bInIsAiming)
{
	bIsAiming = bInIsAiming;
	if (IsValid(OwnerCharacter))
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}

}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (IsValid(EquippedWeapon) && IsValid(OwnerCharacter))
	{
		OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		OwnerCharacter->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;

	if 
	(
		!IsValid(OwnerCharacter) ||
		!IsValid(EquippedWeapon)
	)
	{
		return;
	}

	if (bFireButtonPressed)
	{
		OwnerCharacter->PlayFireMontage(bIsAiming);
		EquippedWeapon->Fire();
	}
}
