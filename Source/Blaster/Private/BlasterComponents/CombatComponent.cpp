#include "BlasterComponents/CombatComponent.h"

#include "Character/BlasterCharacter.h"

#include "Weapon/WeaponBase.h"

#include "Engine/SKeletalMeshSocket.h"

#include "Components/SphereComponent.h"

#include "Net/UnrealNetwork.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);

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
}

