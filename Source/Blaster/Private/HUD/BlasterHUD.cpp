#include "HUD/BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D viewportSize;

	if (IsValid(GEngine))
	{
		GEngine->GameViewport->GetViewportSize(viewportSize);
		const FVector2D viewportCenter(viewportSize.X / 2.f, viewportSize.Y / 2.f);

		if (IsValid(HUDPackage.CrosshairCenter))
		{
			DrawCrosshair(HUDPackage.CrosshairCenter, viewportCenter);
		}

		if (IsValid(HUDPackage.CrosshairLeft))
		{
			DrawCrosshair(HUDPackage.CrosshairLeft, viewportCenter);
		}

		if (IsValid(HUDPackage.CrosshairRight))
		{
			DrawCrosshair(HUDPackage.CrosshairRight, viewportCenter);
		}

		if (IsValid(HUDPackage.CrosshairTop))
		{
			DrawCrosshair(HUDPackage.CrosshairTop, viewportCenter);
		}

		if (IsValid(HUDPackage.CrosshairBottom))
		{
			DrawCrosshair(HUDPackage.CrosshairBottom, viewportCenter);
		}
	}

}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter)
{
	const float textureWidth = Texture->GetSizeX();
	const float textureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint
	(
		ViewportCenter.X - (textureWidth / 2.f),
		ViewportCenter.Y - (textureHeight / 2.f)
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
		FLinearColor::White
	);
}
