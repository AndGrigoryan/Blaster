#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

class UInputMappingContext;
class UInputAction;

class AWeaponBase;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

	UPROPERTY(VisibleAnyWhere, Category = "Camera")
	TObjectPtr<class USpringArmComponent> CameraBoomComponent;

	UPROPERTY(VisibleAnyWhere, Category = "Camera")
	TObjectPtr<class UCameraComponent> FollowCameraComponent;

#pragma region Input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputMappingContext> InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> GamepadLookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> MovementAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> WalkAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> PrimaryAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> SecondaryAction;
#pragma endregion


#pragma region Movement
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector2D TargetInput = FVector2D::ZeroVector;
#pragma endregion

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class APlayerController> PlayerController;

protected:
	virtual void BeginPlay() override;

#pragma region Input
	UFUNCTION(BlueprintCallable)
	void OnLook(const FInputActionValue& Axis);

	UFUNCTION(BlueprintCallable)
	void OnLookGamepad(const FInputActionValue& Axis);

	UFUNCTION()
	void OnMove(const FInputActionValue& Axis);

	UFUNCTION()
	void OnJump();

	UFUNCTION()
	void OnCrouch();

	UFUNCTION()
	void OnInteract();

	UFUNCTION()
	void OnPrimaryActionStart();

	UFUNCTION()
	void OnPrimaryActionEnd();

	UFUNCTION()
	void OnSecondaryActionStart();

	UFUNCTION()
	void OnSecondaryActionEnd();

#pragma endregion


	UFUNCTION()
	void AimOffseet(float DeltaTime);

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	void SetOverlappingWeapon(AWeaponBase* Weapon);

	bool IsWeaponEquipped();

	bool IsAiming();

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> OverheadWidgetComponent;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	TObjectPtr<AWeaponBase> OverlappingWeapon;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCombatComponent> CombatComponent;

	float AO_Yaw;

	float AO_Pitch;

	FRotator StartingAimRotation;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeaponBase* LastWeapon);

	UFUNCTION(Server, Reliable)
	void OnServerInteract();

};
