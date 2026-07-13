#include "BlasterComponents/CombatComponent.h"

#include "Character/BlasterCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Weapon/WeaponBase.h"

#include "Engine/SKeletalMeshSocket.h"

#include "Components/SphereComponent.h"

#include "Net/UnrealNetwork.h"

#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"

#include "PlayerController/BlasterPlayerController.h"

#include "HUD/BlasterHUD.h"

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

	SetHUDCrosshairs(DeltaTime);

	if(IsValid(OwnerCharacter) && OwnerCharacter->IsLocallyControlled())
	{
		FHitResult hitResult;
		TraceUnderCrosshairs(hitResult);
		HitTarget = hitResult.ImpactPoint;
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (!IsValid(OwnerCharacter) ||
		!IsValid(OwnerCharacter->Controller))
	{
		return;
	}

	if (!IsValid(Controller))
	{
		Controller = Cast<ABlasterPlayerController>(OwnerCharacter->Controller);
	}

	if (!IsValid(Controller))
	{
		return;
	}

	if (!IsValid(HUD))
	{
		HUD = Cast<ABlasterHUD>(Controller->GetHUD());
	}

	if (!IsValid(HUD))
	{
		return;
	}

	FHUDPackage HUDPackage{};
	if (IsValid(EquippedWeapon))
	{
		HUDPackage.CrosshairCenter = EquippedWeapon->CrosshairCenter;
		HUDPackage.CrosshairLeft = EquippedWeapon->CrosshairLeft;
		HUDPackage.CrosshairRight = EquippedWeapon->CrosshairRight;
		HUDPackage.CrosshairTop = EquippedWeapon->CrosshairTop;
		HUDPackage.CrosshairBottom = EquippedWeapon->CrosshairBottom;
	}

	// Calcualte crosshair spread

	//[0,600] -> [0, 1]
	FVector2D walkSpeedRange(0.f, OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed);
	FVector2D velocityMultiplierRange(0.f, 1.f);
	FVector velocity = OwnerCharacter->GetVelocity();
	velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(walkSpeedRange, velocityMultiplierRange, velocity.Size());

	if (OwnerCharacter->GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	HUDPackage.CrosshairSpread = CrosshairVelocityFactor + CrosshairInAirFactor;

	HUD->SetHUDPackage(HUDPackage);
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
		FHitResult hitRes;
		TraceUnderCrosshairs(hitRes);
		ServerFire(hitRes.ImpactPoint);
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
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
	EquippedWeapon->Fire(TraceHitTarget);
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
	}
}
