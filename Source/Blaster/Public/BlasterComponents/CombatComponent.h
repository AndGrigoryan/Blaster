#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


class ABlasterCharacter;

class AWeaponBase;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();


protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void Init(ABlasterCharacter* InOwnerCharacter);

	UFUNCTION()
	void EquipWeapon(AWeaponBase* InWeaponToEquip);

private:
	TObjectPtr<ABlasterCharacter> OwnerCharacter;

	TObjectPtr<AWeaponBase> EquippedWeapon;

};
