// Menu de pausa, feito em Slate (sem assets de interface)

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Styling/SlateTypes.h"
#include "Brushes/SlateColorBrush.h"

class SButton;

// PROVISORIO: Menu de pausa, visual simples (fundo escurecido, botoes de cor solida, fonte padrao). Trocar por arte propria.
class SPauseMenuWidget : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SPauseMenuWidget) {}
		/** CONTINUAR (ou Esc/P/Start de novo) */
		SLATE_EVENT(FSimpleDelegate, OnResume)
		/** MENU PRINCIPAL */
		SLATE_EVENT(FSimpleDelegate, OnMainMenu)
		/** SAIR */
		SLATE_EVENT(FSimpleDelegate, OnQuit)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual bool SupportsKeyboardFocus() const override { return true; }

	/** Esc / P / Start fecham o menu e voltam ao jogo */
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	/** Botao que recebe o foco ao abrir o menu (teclado/controle) */
	TSharedPtr<SButton> GetResumeButton() const { return ResumeButton; }

private:

	FReply HandleResume();
	FReply HandleMainMenu();
	FReply HandleQuit();

	TSharedRef<SWidget> MakeMenuButton(const FText& Label, FOnClicked OnClicked, TSharedPtr<SButton>* OutButton = nullptr);

	FSimpleDelegate OnResume;
	FSimpleDelegate OnMainMenu;
	FSimpleDelegate OnQuit;

	TSharedPtr<SButton> ResumeButton;

	FSlateColorBrush OverlayBrush = FSlateColorBrush(FLinearColor(0.f, 0.f, 0.f, 0.65f));
	FButtonStyle MenuButtonStyle;
};
