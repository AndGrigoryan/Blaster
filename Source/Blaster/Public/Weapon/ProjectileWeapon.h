#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "ProjectileWeapon.generated.h"


UCLASS()
class BLASTER_API AProjectileWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;

};
