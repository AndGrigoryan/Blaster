#include "Weapon/ProjectileWeapon.h"

#include "Engine/SkeletalMeshSocket.h"

#include "Weapon/Projectile.h"


void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (!HasAuthority())
	{
		return;
	}

	APawn* instigatorPawn = Cast<APawn>(GetOwner());

	if (!IsValid(instigatorPawn))
	{
		return;
	}

	UWorld* world = GetWorld();

	if (!IsValid(world))
	{
		return;
	}

	USkeletalMeshComponent* weaponMesh = GetWeaponMesh();

	if (!IsValid(weaponMesh))
	{
		return;
	}

	if (!IsValid(ProjectileClass))
	{
		return;
	}

	const USkeletalMeshSocket* muzzleFlashSocket = weaponMesh->GetSocketByName(FName("MuzzleFlash"));

	if (muzzleFlashSocket == nullptr)
	{
		return;
	}

	FTransform socketTransform = muzzleFlashSocket->GetSocketTransform(weaponMesh);
	
	// From Muzzle Flash Socket to Hit Location from TraceUnderCrosshairs 
	FVector toTarget = HitTarget - socketTransform.GetLocation();
	FRotator targetRotation = toTarget.Rotation();

	FActorSpawnParameters spawnParams;

	spawnParams.Owner = GetOwner();
	spawnParams.Instigator = instigatorPawn;

	world->SpawnActor<AProjectile>
		(
		ProjectileClass,
		socketTransform.GetLocation(),
		targetRotation,
		spawnParams
		);

}
