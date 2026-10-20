// Controlador do menu principal: mostra o menu, libera o mouse e abre o jogo

#include "MainMenuPlayerController.h"
#include "MainMenuWidget.h"
#include "Widgets/Input/SButton.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController() || !GEngine || !GEngine->GameViewport)
	{
		return;
	}

	// cria o menu e coloca na tela
	SAssignNew(MenuWidget, SMainMenuWidget)
		.OnPlay(FSimpleDelegate::CreateUObject(this, &AMainMenuPlayerController::StartGame))
		.OnQuit(FSimpleDelegate::CreateUObject(this, &AMainMenuPlayerController::QuitGame));

	GEngine->GameViewport->AddViewportWidgetContent(MenuWidget.ToSharedRef());

	// so interface: mouse visivel, foco no botao JOGAR (para teclado/controle)
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(MenuWidget->GetPlayButton());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void AMainMenuPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MenuWidget.IsValid() && GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(MenuWidget.ToSharedRef());
	}
	MenuWidget.Reset();

	Super::EndPlay(EndPlayReason);
}

void AMainMenuPlayerController::StartGame()
{
	// devolve o controle ao jogo antes de trocar de mapa
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;

	UGameplayStatics::OpenLevel(this, GameplayLevel);
}

void AMainMenuPlayerController::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}
