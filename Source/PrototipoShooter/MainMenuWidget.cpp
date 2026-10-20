// Tela do menu principal, feita em Slate (sem assets de interface)

#include "MainMenuWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Styling/CoreStyle.h"

#define LOCTEXT_NAMESPACE "MainMenu"

void SMainMenuWidget::Construct(const FArguments& InArgs)
{
	OnPlay = InArgs._OnPlay;
	OnQuit = InArgs._OnQuit;

	// botoes: grafite, laranja ao passar o mouse, laranja escuro ao clicar
	MenuButtonStyle = FButtonStyle()
		.SetNormal(FSlateColorBrush(FLinearColor(0.08f, 0.08f, 0.09f, 0.9f)))
		.SetHovered(FSlateColorBrush(FLinearColor(0.85f, 0.35f, 0.05f, 1.f)))
		.SetPressed(FSlateColorBrush(FLinearColor(0.55f, 0.2f, 0.02f, 1.f)))
		.SetNormalPadding(FMargin(0.f))
		.SetPressedPadding(FMargin(0.f));

	// PROVISORIO: Menu principal, titulo provisorio (o jogo ainda nao tem nome).
	const FText Title = LOCTEXT("Title", "PROJETO SHOOTER");
	const FText Subtitle = LOCTEXT("Subtitle", "PROTOTIPO");

	// lista de controles (com acentos via codigo unicode)
	const FText ControlsText = FText::FromString(FString(
		TEXT("WASD - Mover\n")
		TEXT("Mouse - C\u00E2mera\n")
		TEXT("Bot\u00E3o direito - Mirar\n")
		TEXT("Bot\u00E3o esquerdo - Atirar\n")
		TEXT("R - Recarregar\n")
		TEXT("V - Golpe corpo-a-corpo\n")
		TEXT("Q - Trocar de ombro\n")
		TEXT("E - Interagir\n")
		TEXT("Espa\u00E7o - Pular")));

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(&BackgroundBrush)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(FMargin(120.f, 0.f))
		[
			SNew(SVerticalBox)

			// titulo
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(Title)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 64))
				.ColorAndOpacity(FLinearColor(0.95f, 0.95f, 0.95f, 1.f))
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(4.f, 0.f, 0.f, 48.f))
			[
				SNew(STextBlock)
				.Text(Subtitle)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 18))
				.ColorAndOpacity(FLinearColor(0.85f, 0.35f, 0.05f, 1.f))
			]

			// botoes
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(0.f, 0.f, 0.f, 12.f))
			[
				MakeMenuButton(LOCTEXT("Play", "JOGAR"), FOnClicked::CreateSP(this, &SMainMenuWidget::HandlePlay), &PlayButton)
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(0.f, 0.f, 0.f, 12.f))
			[
				MakeMenuButton(LOCTEXT("Controls", "CONTROLES"), FOnClicked::CreateSP(this, &SMainMenuWidget::HandleControls))
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(0.f, 0.f, 0.f, 24.f))
			[
				MakeMenuButton(LOCTEXT("Quit", "SAIR"), FOnClicked::CreateSP(this, &SMainMenuWidget::HandleQuit))
			]

			// painel de controles (abre/fecha no botao CONTROLES)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBorder)
				.Visibility(this, &SMainMenuWidget::GetControlsVisibility)
				.BorderImage(&PanelBrush)
				.Padding(FMargin(20.f, 16.f))
				[
					SNew(STextBlock)
					.Text(ControlsText)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
					.ColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.8f, 1.f))
				]
			]
		]
	];
}

TSharedRef<SWidget> SMainMenuWidget::MakeMenuButton(const FText& Label, FOnClicked OnClicked, TSharedPtr<SButton>* OutButton)
{
	TSharedPtr<SButton> Button;

	TSharedRef<SWidget> Result =
		SNew(SBox)
		.WidthOverride(320.f)
		[
			SAssignNew(Button, SButton)
			.ButtonStyle(&MenuButtonStyle)
			.ContentPadding(FMargin(24.f, 12.f))
			.OnClicked(OnClicked)
			[
				SNew(STextBlock)
				.Text(Label)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 22))
				.ColorAndOpacity(FLinearColor::White)
			]
		];

	if (OutButton)
	{
		*OutButton = Button;
	}

	return Result;
}

FReply SMainMenuWidget::HandlePlay()
{
	OnPlay.ExecuteIfBound();
	return FReply::Handled();
}

FReply SMainMenuWidget::HandleControls()
{
	bShowControls = !bShowControls;
	return FReply::Handled();
}

FReply SMainMenuWidget::HandleQuit()
{
	OnQuit.ExecuteIfBound();
	return FReply::Handled();
}

EVisibility SMainMenuWidget::GetControlsVisibility() const
{
	return bShowControls ? EVisibility::Visible : EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE
