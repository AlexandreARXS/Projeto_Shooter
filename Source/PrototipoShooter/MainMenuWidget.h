// Tela do menu principal, feita em Slate (sem assets de interface)

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Styling/SlateTypes.h"
#include "Brushes/SlateColorBrush.h"

class SButton;

// PROVISORIO: Menu principal, visual simples (fundo liso, botoes de cor solida, fonte padrao). Trocar por arte, fundo animado/cena 3D e musica.
class SMainMenuWidget : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SMainMenuWidget) {}
		/** Chamado ao clicar em JOGAR */
		SLATE_EVENT(FSimpleDelegate, OnPlay)
		/** Chamado ao clicar em SAIR */
		SLATE_EVENT(FSimpleDelegate, OnQuit)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual bool SupportsKeyboardFocus() const override { return true; }

	/** Botao que recebe o foco ao abrir o menu (teclado/controle) */
	TSharedPtr<SButton> GetPlayButton() const { return PlayButton; }

private:

	FReply HandlePlay();
	FReply HandleControls();
	FReply HandleQuit();

	/** Cria um botao do menu com o estilo padrao */
	TSharedRef<SWidget> MakeMenuButton(const FText& Label, FOnClicked OnClicked, TSharedPtr<SButton>* OutButton = nullptr);

	EVisibility GetControlsVisibility() const;

	FSimpleDelegate OnPlay;
	FSimpleDelegate OnQuit;

	TSharedPtr<SButton> PlayButton;
	bool bShowControls = false;

	FSlateColorBrush BackgroundBrush = FSlateColorBrush(FLinearColor(0.02f, 0.02f, 0.025f, 1.f));
	FSlateColorBrush PanelBrush = FSlateColorBrush(FLinearColor(0.f, 0.f, 0.f, 0.45f));
	FButtonStyle MenuButtonStyle;
};
