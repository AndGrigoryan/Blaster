#include "HUD/BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D viewportSize;

	if (IsValid(GEngine))
	{
		GEngine->GameViewport->GetViewportSize(viewportSize);
		const FVector2D viewportCenter(viewportSize.X / 2.f, viewportSize.Y / 2.f);

		float spreadScale = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if (IsValid(HUDPackage.CrosshairCenter))
		{
			FVector2D spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CrosshairCenter, viewportCenter, spread, HUDPackage.CrosshairsColor);
		}

		if (IsValid(HUDPackage.CrosshairLeft))
		{
			FVector2D spread(-spreadScale, 0.f);
			DrawCrosshair(HUDPackage.CrosshairLeft, viewportCenter, spread, HUDPackage.CrosshairsColor);
		}

		if (IsValid(HUDPackage.CrosshairRight))
		{
			FVector2D spread(spreadScale, 0.f);
			DrawCrosshair(HUDPackage.CrosshairRight, viewportCenter, spread, HUDPackage.CrosshairsColor);
		}

		if (IsValid(HUDPackage.CrosshairTop))
		{
			FVector2D spread(0.f , -spreadScale);
			DrawCrosshair(HUDPackage.CrosshairTop, viewportCenter, spread, HUDPackage.CrosshairsColor);
		}

		if (IsValid(HUDPackage.CrosshairBottom))
		{
			FVector2D spread(0.f, spreadScale);
			DrawCrosshair(HUDPackage.CrosshairBottom, viewportCenter, spread, HUDPackage.CrosshairsColor);
		}
	}

}

void ABlasterHUD::DrawCrosshair
(
	UTexture2D* Texture, 
	FVector2D ViewportCenter, 
	FVector2D Spread,
	FLinearColor CrosshairColor
)
{
	const float textureWidth = Texture->GetSizeX();
	const float textureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint
	(
		ViewportCenter.X - (textureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (textureHeight / 2.f) + Spread.Y
	);

	DrawTexture
	(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		textureWidth,
		textureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrosshairColor
	);
}
