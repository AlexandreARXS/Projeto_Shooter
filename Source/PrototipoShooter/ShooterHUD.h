// HUD do shooter: mira, marcador de acerto, aviso de interacao e mensagens

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

class APrototipoShooterCharacter;

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

public:

	virtual void DrawHUD() override;

private:

	void DrawCrosshair(const APrototipoShooterCharacter* ShooterCharacter);
	void DrawInteraction(const APrototipoShooterCharacter* ShooterCharacter);
	void DrawCenteredText(const FString& Text, float CenterY, const FLinearColor& Color, float Scale);
};
