#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
	
public:
	UPROPERTY();
	TObjectPtr<UTexture2D> CrosshairCenter = nullptr;

	UPROPERTY();
	TObjectPtr<UTexture2D> CrosshairLeft = nullptr;

	UPROPERTY();
	TObjectPtr<UTexture2D> CrosshairRight = nullptr;

	UPROPERTY();
	TObjectPtr<UTexture2D> CrosshairTop = nullptr;

	UPROPERTY();
	TObjectPtr<UTexture2D> CrosshairBottom = nullptr;

	UPROPERTY();
	float CrosshairSpread = 0.f;

	UPROPERTY();
	FLinearColor CrosshairsColor = FLinearColor();
};

UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	FORCEINLINE void SetHUDPackage(const FHUDPackage Package){HUDPackage = Package;}

private:
	FHUDPackage HUDPackage;

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
	
	void DrawCrosshair
	(
		UTexture2D* Texture, 
		FVector2D ViewportCenter, 
		FVector2D Spread,
		FLinearColor CrosshairColor
	);
};
