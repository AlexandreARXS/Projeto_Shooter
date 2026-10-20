// Controlador do menu principal: mostra o menu, libera o mouse e abre o jogo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

class SMainMenuWidget;

UCLASS()
class AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** Mapa aberto ao clicar em JOGAR */
	UPROPERTY(EditAnywhere, Category="Menu")
	FName GameplayLevel = TEXT("/Game/ThirdPerson/Lvl_ThirdPerson");

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	void StartGame();
	void QuitGame();

	TSharedPtr<SMainMenuWidget> MenuWidget;
};
