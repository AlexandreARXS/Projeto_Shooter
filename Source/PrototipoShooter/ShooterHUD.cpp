// HUD do shooter: mira, marcador de acerto, aviso de interacao e mensagens

#include "ShooterHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "PrototipoShooterCharacter.h"
#include "ShooterInteractable.h"

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	const APrototipoShooterCharacter* ShooterCharacter = Cast<APrototipoShooterCharacter>(GetOwningPawn());
	if (!ShooterCharacter)
	{
		return;
	}

	DrawInteraction(ShooterCharacter);
	DrawCrosshair(ShooterCharacter);
}

void AShooterHUD::DrawCenteredText(const FString& Text, float CenterY, const FLinearColor& Color, float Scale)
{
	UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;

	float Width = 0.f;
	float Height = 0.f;
	GetTextSize(Text, Width, Height, Font, Scale);

	const float X = (Canvas->ClipX - Width) * 0.5f;
	const float Y = CenterY - Height * 0.5f;

	// sombra para ler em qualquer fundo
	DrawText(Text, FLinearColor(0.f, 0.f, 0.f, 0.8f), X + 2.f, Y + 2.f, Font, Scale);
	DrawText(Text, Color, X, Y, Font, Scale);
}

void AShooterHUD::DrawInteraction(const APrototipoShooterCharacter* ShooterCharacter)
{
	// mensagem ativa (ex.: depois de interagir)
	FString Message;
	if (ShooterCharacter->GetActiveScreenMessage(Message))
	{
		DrawCenteredText(Message, Canvas->ClipY * 0.3f, FLinearColor::White, 1.6f);
	}

	// aviso de interacao quando ha um objeto ao alcance
	if (const AShooterInteractable* Interactable = ShooterCharacter->GetFocusedInteractable())
	{
		DrawCenteredText(Interactable->Prompt, Canvas->ClipY * 0.68f, FLinearColor(0.6f, 0.9f, 1.f, 1.f), 1.f);
	}
}

void AShooterHUD::DrawCrosshair(const APrototipoShooterCharacter* ShooterCharacter)
{
	// a mira aparece quando a arma esta levantada (mirando ou atirando)
	const float Alpha = ShooterCharacter->GetCombatAlpha();
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

	// marcador de acerto: um X vermelho rapido em volta da mira
	const float SinceHit = GetWorld()->GetTimeSeconds() - ShooterCharacter->GetLastHitConfirmTime();
	if (SinceHit >= 0.f && SinceHit < HitMarkerDuration)
	{
		FLinearColor HitColor = HitMarkerColor;
		HitColor.A *= 1.f - (SinceHit / HitMarkerDuration);

		const float In = 8.f;
		const float Out = 16.f;
		DrawLine(CenterX - Out, CenterY - Out, CenterX - In, CenterY - In, HitColor, 2.5f);
		DrawLine(CenterX + In, CenterY - In, CenterX + Out, CenterY - Out, HitColor, 2.5f);
		DrawLine(CenterX - Out, CenterY + Out, CenterX - In, CenterY + In, HitColor, 2.5f);
		DrawLine(CenterX + In, CenterY + In, CenterX + Out, CenterY + Out, HitColor, 2.5f);
	}
}
