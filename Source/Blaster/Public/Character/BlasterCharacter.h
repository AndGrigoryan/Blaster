#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

class UInputMappingContext;
class UInputAction;

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
#pragma endregion

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> OverheadWidgetComponent;
};
