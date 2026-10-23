// Colecionavel: segure o botao de interacao para coletar; some da cena e aparece um icone na HUD

#pragma once

#include "CoreMinimal.h"
#include "ShooterInteractable.h"
#include "ShooterCollectible.generated.h"

// PROVISORIO: Colecionavel, visual de medalha feita com cilindro basico. Trocar por modelo 3D (ex.: plaqueta, documento, peca) + som e efeito ao coletar.
UCLASS()
class AShooterCollectible : public AShooterInteractable
{
	GENERATED_BODY()

public:

	AShooterCollectible();

	virtual void Tick(float DeltaSeconds) override;

	/** Coleta: avisa o jogador e remove o objeto da cena */
	virtual void Interact(APrototipoShooterCharacter* Interactor) override;

	/** Nome do colecionavel (aparece na HUD ao coletar) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Colecionavel")
	FString CollectibleName = TEXT("Medalha");

protected:

	/** Voltas por segundo do giro */
	UPROPERTY(EditAnywhere, Category="Colecionavel")
	float SpinSpeed = 0.5f;

	/** Altura e velocidade da flutuacao */
	UPROPERTY(EditAnywhere, Category="Colecionavel")
	float BobHeight = 8.f;

	UPROPERTY(EditAnywhere, Category="Colecionavel")
	float BobSpeed = 2.f;

	/** Altura do centro da medalha em relacao ao chao */
	UPROPERTY(EditAnywhere, Category="Colecionavel")
	float FloatHeight = 70.f;

private:

	float RunningTime = 0.f;
};
