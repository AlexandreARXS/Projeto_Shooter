// HUD do shooter: mira, marcador de acerto, aviso de interacao, mensagens, pontos e municao

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

class APrototipoShooterCharacter;

// PROVISORIO: HUD, desenhada direto no Canvas com a fonte padrao da engine. Trocar por widgets UMG com arte propria.
UCLASS()
class AShooterHUD : public AHUD
{
	GENERATED_BODY()

protected:

	/** Cor da mira */
	UPROPERTY(EditAnywhere, Category="Mira")
	FLinearColor CrosshairColor = FLinearColor(1.f, 1.f, 1.f, 0.9f);

	/** Distancia entre o centro e o inicio de cada traco */
	UPROPERTY(EditAnywhere, Category="Mira")
	float CrosshairGap = 6.f;

	/** Comprimento de cada traco */
	UPROPERTY(EditAnywhere, Category="Mira")
	float CrosshairLength = 10.f;

	/** Espessura dos tracos */
	UPROPERTY(EditAnywhere, Category="Mira")
	float CrosshairThickness = 2.f;

	/** Cor do marcador de acerto */
	UPROPERTY(EditAnywhere, Category="Mira")
	FLinearColor HitMarkerColor = FLinearColor(1.f, 0.15f, 0.1f, 1.f);

	/** Quanto tempo o marcador de acerto fica na tela */
	UPROPERTY(EditAnywhere, Category="Mira")
	float HitMarkerDuration = 0.25f;

	/** Distancia dos paineis ate a borda da tela */
	UPROPERTY(EditAnywhere, Category="Painel")
	float HudMargin = 40.f;

	/** Duracao do clarao vermelho ao levar dano */
	UPROPERTY(EditAnywhere, Category="Painel")
	float DamageFlashDuration = 0.35f;

	/** Abaixo deste numero de balas, o contador fica laranja */
	UPROPERTY(EditAnywhere, Category="Painel")
	int32 LowAmmoThreshold = 10;

public:

	virtual void DrawHUD() override;

private:

	void DrawCrosshair(const APrototipoShooterCharacter* ShooterCharacter);
	void DrawInteraction(const APrototipoShooterCharacter* ShooterCharacter);
	void DrawCenteredText(const FString& Text, float CenterY, const FLinearColor& Color, float Scale);
	void DrawScore(const APrototipoShooterCharacter* ShooterCharacter);
	void DrawEnemyHealthBars();
	void DrawCollectibles(const APrototipoShooterCharacter* ShooterCharacter);
	void DrawPlayerHealth(const APrototipoShooterCharacter* ShooterCharacter);
	void DrawDamageFeedback(const APrototipoShooterCharacter* ShooterCharacter);
	void DrawAmmo(const APrototipoShooterCharacter* ShooterCharacter);
	void DrawTextAt(const FString& Text, float X, float Y, const FLinearColor& Color, float Scale, bool bAlignRight);
};
