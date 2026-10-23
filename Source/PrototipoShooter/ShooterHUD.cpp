// HUD do shooter: mira, marcador de acerto, aviso de interacao e mensagens

#include "ShooterHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/Texture.h"
#include "PrototipoShooterCharacter.h"
#include "ShooterInteractable.h"
#include "ShooterEnemy.h"
#include "EngineUtils.h"

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

	DrawEnemyHealthBars();
	DrawInteraction(ShooterCharacter);
	DrawCrosshair(ShooterCharacter);
	DrawScore(ShooterCharacter);
	DrawAmmo(ShooterCharacter);
	DrawCollectibles(ShooterCharacter);
	DrawPlayerHealth(ShooterCharacter);
	DrawDamageFeedback(ShooterCharacter);
}

void AShooterHUD::DrawPlayerHealth(const APrototipoShooterCharacter* ShooterCharacter)
{
	// canto inferior esquerdo: barra verde de vida
	const float Percent = FMath::Clamp(ShooterCharacter->GetHealth() / ShooterCharacter->GetMaxHealth(), 0.f, 1.f);

	const float BarWidth = 220.f;
	const float BarHeight = 12.f;
	const float X = HudMargin;
	const float BarY = Canvas->ClipY - HudMargin - BarHeight;

	DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.35f), X - 12.f, BarY - 34.f, BarWidth + 24.f, BarHeight + 46.f);
	DrawTextAt(TEXT("VIDA"), X, BarY - 26.f, FLinearColor(0.75f, 0.75f, 0.75f, 1.f), 0.9f, false);

	DrawRect(FLinearColor(1.f, 1.f, 1.f, 0.15f), X, BarY, BarWidth, BarHeight);
	DrawRect(FLinearColor(0.2f, 0.85f, 0.25f, 1.f), X, BarY, BarWidth * Percent, BarHeight);
}

void AShooterHUD::DrawDamageFeedback(const APrototipoShooterCharacter* ShooterCharacter)
{
	// PROVISORIO: Vida, feedback de dano com a tela inteira avermelhada. Trocar por vinheta/sangue nas bordas + som.
	const float SinceDamage = GetWorld()->GetTimeSeconds() - ShooterCharacter->GetLastDamageTime();
	if (SinceDamage >= 0.f && SinceDamage < DamageFlashDuration)
	{
		const float Alpha = 0.3f * (1.f - SinceDamage / DamageFlashDuration);
		DrawRect(FLinearColor(0.8f, 0.f, 0.f, Alpha), 0.f, 0.f, Canvas->ClipX, Canvas->ClipY);
	}

	// tela de morte
	if (ShooterCharacter->IsDead())
	{
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.5f), 0.f, 0.f, Canvas->ClipX, Canvas->ClipY);
		DrawCenteredText(FString(TEXT("VOC\u00CA MORREU")), Canvas->ClipY * 0.42f, FLinearColor(0.9f, 0.1f, 0.1f, 1.f), 2.5f);
		DrawCenteredText(TEXT("Reiniciando..."), Canvas->ClipY * 0.52f, FLinearColor(0.8f, 0.8f, 0.8f, 1.f), 1.f);
	}
}

void AShooterHUD::DrawCollectibles(const APrototipoShooterCharacter* ShooterCharacter)
{
	const int32 Count = ShooterCharacter->GetCollectedCount();
	if (Count <= 0)
	{
		return;
	}

	// PROVISORIO: Colecionavel, icone desenhado como hexagono dourado. Trocar por icone com arte propria.
	const float SinceCollect = GetWorld()->GetTimeSeconds() - ShooterCharacter->GetLastCollectTime();

	// canto superior direito
	const float CenterX = Canvas->ClipX - HudMargin - 18.f;
	const float CenterY = HudMargin + 22.f;

	// "pulo" do icone logo apos coletar
	const float Pop = SinceCollect < 0.4f ? 1.f + 0.6f * FMath::Sin((SinceCollect / 0.4f) * PI) : 1.f;
	const float Radius = 16.f * Pop;

	// fundo + medalha (hexagono dourado com miolo mais claro)
	DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.35f), CenterX - 30.f - 60.f, CenterY - 26.f, 120.f, 52.f);
	Canvas->K2_DrawPolygon(nullptr, FVector2D(CenterX, CenterY), FVector2D(Radius, Radius), 6, FLinearColor(0.85f, 0.6f, 0.08f, 1.f));
	Canvas->K2_DrawPolygon(nullptr, FVector2D(CenterX, CenterY), FVector2D(Radius * 0.55f, Radius * 0.55f), 6, FLinearColor(1.f, 0.85f, 0.35f, 1.f));

	// quantidade
	DrawTextAt(FString::Printf(TEXT("x%d"), Count), CenterX - Radius - 8.f, CenterY - 12.f, FLinearColor::White, 1.2f, true);

	// aviso rapido logo apos coletar
	if (SinceCollect < 2.5f)
	{
		const float Alpha = SinceCollect < 2.f ? 1.f : 1.f - (SinceCollect - 2.f) / 0.5f;
		const FString Text = FString::Printf(TEXT("%s coletada!"), *ShooterCharacter->GetLastCollectedName());
		DrawTextAt(Text, Canvas->ClipX - HudMargin, CenterY + 32.f, FLinearColor(1.f, 0.85f, 0.35f, Alpha), 1.f, true);
	}
}

void AShooterHUD::DrawTextAt(const FString& Text, float X, float Y, const FLinearColor& Color, float Scale, bool bAlignRight)
{
	UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;

	if (bAlignRight)
	{
		float Width = 0.f;
		float Height = 0.f;
		GetTextSize(Text, Width, Height, Font, Scale);
		X -= Width;
	}

	// sombra para ler em qualquer fundo
	DrawText(Text, FLinearColor(0.f, 0.f, 0.f, 0.8f), X + 2.f, Y + 2.f, Font, Scale);
	DrawText(Text, Color, X, Y, Font, Scale);
}

void AShooterHUD::DrawScore(const APrototipoShooterCharacter* ShooterCharacter)
{
	// canto superior esquerdo
	const float X = HudMargin;
	const float Y = HudMargin;

	DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.35f), X - 12.f, Y - 8.f, 230.f, 62.f);
	DrawTextAt(TEXT("PONTOS"), X, Y, FLinearColor(0.75f, 0.75f, 0.75f, 1.f), 0.9f, false);
	DrawTextAt(FString::FromInt(ShooterCharacter->GetScore()), X, Y + 18.f, FLinearColor(1.f, 0.85f, 0.2f, 1.f), 1.8f, false);
}

void AShooterHUD::DrawAmmo(const APrototipoShooterCharacter* ShooterCharacter)
{
	// canto inferior direito
	const float Right = Canvas->ClipX - HudMargin;
	const float Bottom = Canvas->ClipY - HudMargin;

	const int32 Ammo = ShooterCharacter->GetCurrentAmmo();
	const int32 Magazine = ShooterCharacter->GetMagazineSize();

	DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.35f), Right - 230.f, Bottom - 92.f, 242.f, 100.f);

	// cor das balas: branco, laranja quando esta acabando, vermelho quando vazio
	FLinearColor AmmoColor = FLinearColor::White;
	if (Ammo <= 0)
	{
		AmmoColor = FLinearColor(1.f, 0.2f, 0.15f, 1.f);
	}
	else if (Ammo <= LowAmmoThreshold)
	{
		AmmoColor = FLinearColor(1.f, 0.6f, 0.1f, 1.f);
	}

	// "32 / 40"
	DrawTextAt(FString::Printf(TEXT("/ %d"), Magazine), Right, Bottom - 50.f, FLinearColor(0.75f, 0.75f, 0.75f, 1.f), 1.2f, true);
	DrawTextAt(FString::FromInt(Ammo), Right - 62.f, Bottom - 74.f, AmmoColor, 2.4f, true);

	// recarga: texto e barra de progresso
	if (ShooterCharacter->IsReloading())
	{
		const float BarWidth = 206.f;
		const float BarHeight = 6.f;
		const float BarX = Right - BarWidth;
		const float BarY = Bottom - 6.f;

		DrawTextAt(TEXT("RECARREGANDO"), Right - BarWidth, Bottom - 24.f, FLinearColor(1.f, 0.85f, 0.2f, 1.f), 0.9f, false);
		DrawRect(FLinearColor(1.f, 1.f, 1.f, 0.2f), BarX, BarY, BarWidth, BarHeight);
		DrawRect(FLinearColor(1.f, 0.85f, 0.2f, 1.f), BarX, BarY, BarWidth * ShooterCharacter->GetReloadProgress(), BarHeight);
	}
	else if (Ammo < Magazine)
	{
		DrawTextAt(TEXT("[R] Recarregar"), Right - 206.f, Bottom - 24.f, FLinearColor(0.75f, 0.75f, 0.75f, 0.8f), 0.9f, false);
	}
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

	// indicador de cobertura
	if (ShooterCharacter->IsInCover())
	{
		DrawCenteredText(ShooterCharacter->IsAiming() ? TEXT("COBERTURA - MIRANDO") : TEXT("EM COBERTURA"), Canvas->ClipY * 0.9f, FLinearColor(0.7f, 0.7f, 0.7f, 0.9f), 0.9f);
	}

	// aviso de interacao quando ha um objeto ao alcance
	if (const AShooterInteractable* Interactable = ShooterCharacter->GetFocusedInteractable())
	{
		const float PromptY = Canvas->ClipY * 0.68f;
		DrawCenteredText(Interactable->Prompt, PromptY, FLinearColor(0.6f, 0.9f, 1.f, 1.f), 1.f);

		// barra de progresso para objetos que exigem segurar o botao
		if (Interactable->HoldDuration > 0.f)
		{
			const float BarWidth = 160.f;
			const float BarHeight = 6.f;
			const float BarX = (Canvas->ClipX - BarWidth) * 0.5f;
			const float BarY = PromptY + 18.f;

			DrawRect(FLinearColor(1.f, 1.f, 1.f, 0.2f), BarX, BarY, BarWidth, BarHeight);
			DrawRect(FLinearColor(1.f, 0.8f, 0.2f, 1.f), BarX, BarY, BarWidth * ShooterCharacter->GetInteractHoldProgress(), BarHeight);
		}
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

	// marcador de acerto: X em volta da mira (vermelho = corpo, amarelo = cabeca, maior = abate)
	const float Now = GetWorld()->GetTimeSeconds();
	const float SinceHit = Now - ShooterCharacter->GetLastHitConfirmTime();
	const bool bRecentKill = (Now - ShooterCharacter->GetLastKillTime()) < 0.5f;
	if (SinceHit >= 0.f && (SinceHit < HitMarkerDuration || bRecentKill))
	{
		FLinearColor HitColor = ShooterCharacter->WasLastHitHeadshot() ? FLinearColor(1.f, 0.85f, 0.1f, 1.f) : HitMarkerColor;
		HitColor.A *= bRecentKill ? 1.f : 1.f - (SinceHit / HitMarkerDuration);

		const float In = bRecentKill ? 10.f : 8.f;
		const float Out = bRecentKill ? 24.f : 16.f;
		DrawLine(CenterX - Out, CenterY - Out, CenterX - In, CenterY - In, HitColor, 2.5f);
		DrawLine(CenterX + In, CenterY - In, CenterX + Out, CenterY - Out, HitColor, 2.5f);
		DrawLine(CenterX - Out, CenterY + Out, CenterX - In, CenterY + In, HitColor, 2.5f);
		DrawLine(CenterX + In, CenterY + In, CenterX + Out, CenterY + Out, HitColor, 2.5f);
	}
}

void AShooterHUD::DrawEnemyHealthBars()
{
	if (!PlayerOwner)
	{
		return;
	}

	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerOwner->GetPlayerViewPoint(CameraLocation, CameraRotation);
	const FVector CameraForward = CameraRotation.Vector();

	for (TActorIterator<AShooterEnemy> It(GetWorld()); It; ++It)
	{
		const AShooterEnemy* Enemy = *It;
		if (!Enemy || Enemy->IsDead())
		{
			continue;
		}

		// so desenha se estiver na frente da camera e nao muito longe
		const FVector WorldLocation = Enemy->GetHealthBarLocation();
		const FVector ToEnemy = WorldLocation - CameraLocation;
		if (FVector::DotProduct(ToEnemy, CameraForward) <= 0.f || ToEnemy.Size() > 4000.f)
		{
			continue;
		}

		const FVector Screen = Project(WorldLocation);
		const float Percent = FMath::Clamp(Enemy->GetHealth() / Enemy->GetMaxHealth(), 0.f, 1.f);

		const float Width = 80.f;
		const float Height = 7.f;
		const float X = Screen.X - Width * 0.5f;
		const float Y = Screen.Y;

		// fundo escuro + vida (verde -> vermelho)
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.6f), X - 1.f, Y - 1.f, Width + 2.f, Height + 2.f);
		const FLinearColor BarColor = FLinearColor::LerpUsingHSV(FLinearColor(1.f, 0.1f, 0.1f), FLinearColor(0.2f, 1.f, 0.2f), Percent);
		DrawRect(BarColor, X, Y, Width * Percent, Height);

		// inimigo em combate (viu o jogador): "!" vermelho acima da barra
		if (Enemy->IsEngaged())
		{
			DrawTextAt(TEXT("!"), Screen.X - 5.f, Y - 30.f, FLinearColor(1.f, 0.15f, 0.1f, 1.f), 1.8f, false);
		}
	}
}
