#include "BlasterComponents/CombatComponent.h"

#include "Character/BlasterCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Weapon/WeaponBase.h"

#include "Engine/SKeletalMeshSocket.h"

#include "Components/SphereComponent.h"

#include "Net/UnrealNetwork.h"

#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"



UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.f;

	AimWalkSpeed = 450.f;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);

	DOREPLIFETIME(UCombatComponent, bIsAiming);
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

	FHitResult hitRes;
	TraceUnderCrosshairs(hitRes);

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

	if (bFireButtonPressed)
	{
		ServerFire();
	}
}

void UCombatComponent::ServerFire_Implementation()
{
	MulticastFire();
}

void UCombatComponent::MulticastFire_Implementation()
{
	if
		(
		!IsValid(OwnerCharacter) ||
		!IsValid(EquippedWeapon)
		)
	{
		return;
	}

	OwnerCharacter->PlayFireMontage(bIsAiming);
	EquippedWeapon->Fire();
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& OutTraceHitResult)
{
	FVector2D viewportSize;

	if (IsValid(GEngine) && IsValid(GEngine->GameViewport))
	{
		GEngine->GameViewport->GetViewportSize(viewportSize);
	}

	FVector2D crosshairLocation(viewportSize.X / 2.f, viewportSize.Y / 2.f);

	FVector crosshairWorldPosition;
	FVector crosshairWorldDirection;

	bool bscreenToWorld = UGameplayStatics::DeprojectScreenToWorld
	(
		UGameplayStatics::GetPlayerController(this, 0),
		crosshairLocation,
		crosshairWorldPosition,
		crosshairWorldDirection
	);

	if (bscreenToWorld)
	{
		FVector start = crosshairWorldPosition;

		FVector end = start + crosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel
		(
			OutTraceHitResult,
			start,
			end,
			ECollisionChannel::ECC_Visibility
		);

		if (!OutTraceHitResult.bBlockingHit)
		{
			OutTraceHitResult.ImpactPoint = end;
		}
		else
		{
			DrawDebugSphere
			(
				GetWorld(),
				OutTraceHitResult.ImpactPoint,
				12.f,
				12,
				FColor::Red
			);
		}
	}
}