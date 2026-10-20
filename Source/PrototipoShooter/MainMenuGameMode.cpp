// GameMode do menu principal: sem personagem, so o controlador do menu

#include "MainMenuGameMode.h"
#include "MainMenuPlayerController.h"
#include "GameFramework/HUD.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	PlayerControllerClass = AMainMenuPlayerController::StaticClass();
	DefaultPawnClass = nullptr;
	HUDClass = AHUD::StaticClass();
}
