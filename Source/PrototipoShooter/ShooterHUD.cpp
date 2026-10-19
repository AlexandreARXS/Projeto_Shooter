// HUD do shooter: desenha a mira no centro da tela

#include "ShooterHUD.h"
#include "Engine/Canvas.h"
#include "PrototipoShooterCharacter.h"

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	// so mostra a mira enquanto o personagem esta mirando
	const APrototipoShooterCharacter* ShooterCharacter = Cast<APrototipoShooterCharacter>(GetOwningPawn());
	if (!ShooterCharacter)
	{
		return;
	}

	const float Alpha = ShooterCharacter->GetAimAlpha();
	if (Alpha <= 0.05f)
	{
		return;
	}

	// a mira aparece suavemente junto com o zoom
	FLinearColor Color = CrosshairColor;
	Color.A *= Alpha;

	const float CenterX = Canvas->ClipX * 0.5f;
	const float CenterY = Canvas->ClipY * 0.5f;
	const float Gap = CrosshairGap;
	const float Len = CrosshairLength;

	// quatro tracos em cruz, com um ponto no meio
	DrawLine(CenterX - Gap - Len, CenterY, CenterX - Gap, CenterY, Color, CrosshairThickness);
	DrawLine(CenterX + Gap, CenterY, CenterX + Gap + Len, CenterY, Color, CrosshairThickness);
	DrawLine(CenterX, CenterY - Gap - Len, CenterX, CenterY - Gap, Color, CrosshairThickness);
	DrawLine(CenterX, CenterY + Gap, CenterX, CenterY + Gap + Len, Color, CrosshairThickness);
	DrawRect(Color, CenterX - 1.f, CenterY - 1.f, 2.f, 2.f);
}
