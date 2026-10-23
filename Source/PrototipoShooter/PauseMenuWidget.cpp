// Menu de pausa, feito em Slate (sem assets de interface)

#include "PauseMenuWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Styling/CoreStyle.h"
#include "InputCoreTypes.h"

#define LOCTEXT_NAMESPACE "PauseMenu"

void SPauseMenuWidget::Construct(const FArguments& InArgs)
{
	OnResume = InArgs._OnResume;
	OnMainMenu = InArgs._OnMainMenu;
	OnQuit = InArgs._OnQuit;

	// mesmo estilo de botao do menu principal
	MenuButtonStyle = FButtonStyle()
		.SetNormal(FSlateColorBrush(FLinearColor(0.08f, 0.08f, 0.09f, 0.9f)))
		.SetHovered(FSlateColorBrush(FLinearColor(0.85f, 0.35f, 0.05f, 1.f)))
		.SetPressed(FSlateColorBrush(FLinearColor(0.55f, 0.2f, 0.02f, 1.f)))
		.SetNormalPadding(FMargin(0.f))
		.SetPressedPadding(FMargin(0.f));

	ChildSlot
	[
		// fundo escurecido por cima do jogo pausado
		SNew(SBorder)
		.BorderImage(&OverlayBrush)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(FMargin(0.f, 0.f, 0.f, 40.f))
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Title", "PAUSADO"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 48))
				.ColorAndOpacity(FLinearColor(0.95f, 0.95f, 0.95f, 1.f))
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(0.f, 0.f, 0.f, 12.f))
			[
				MakeMenuButton(LOCTEXT("Resume", "CONTINUAR"), FOnClicked::CreateSP(this, &SPauseMenuWidget::HandleResume), &ResumeButton)
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(0.f, 0.f, 0.f, 12.f))
			[
				MakeMenuButton(LOCTEXT("MainMenu", "MENU PRINCIPAL"), FOnClicked::CreateSP(this, &SPauseMenuWidget::HandleMainMenu))
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				MakeMenuButton(LOCTEXT("Quit", "SAIR"), FOnClicked::CreateSP(this, &SPauseMenuWidget::HandleQuit))
			]
		]
	];
}

TSharedRef<SWidget> SPauseMenuWidget::MakeMenuButton(const FText& Label, FOnClicked OnClicked, TSharedPtr<SButton>* OutButton)
{
	TSharedPtr<SButton> Button;

	TSharedRef<SWidget> Result =
		SNew(SBox)
		.WidthOverride(320.f)
		[
			SAssignNew(Button, SButton)
			.ButtonStyle(&MenuButtonStyle)
			.ContentPadding(FMargin(24.f, 12.f))
			.HAlign(HAlign_Center)
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

FReply SPauseMenuWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();
	if (Key == EKeys::Escape || Key == EKeys::P || Key == EKeys::Gamepad_Special_Right)
	{
		return HandleResume();
	}

	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}

FReply SPauseMenuWidget::HandleResume()
{
	OnResume.ExecuteIfBound();
	return FReply::Handled();
}

FReply SPauseMenuWidget::HandleMainMenu()
{
	OnMainMenu.ExecuteIfBound();
	return FReply::Handled();
}

FReply SPauseMenuWidget::HandleQuit()
{
	OnQuit.ExecuteIfBound();
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
