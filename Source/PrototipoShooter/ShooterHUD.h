// HUD do shooter: desenha a mira no centro da tela

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

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
};
