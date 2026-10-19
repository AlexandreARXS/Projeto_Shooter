// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrototipoShooterGameMode.h"
#include "ShooterHUD.h"

APrototipoShooterGameMode::APrototipoShooterGameMode()
{
	// HUD com a mira na tela
	HUDClass = AShooterHUD::StaticClass();
}
