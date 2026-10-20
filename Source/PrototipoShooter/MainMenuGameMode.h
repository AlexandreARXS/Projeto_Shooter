// GameMode do menu principal: sem personagem, so o controlador do menu
// Usado automaticamente em mapas cujo nome comeca com "MainMenu_" (ver DefaultEngine.ini)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

UCLASS()
class AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AMainMenuGameMode();
};
