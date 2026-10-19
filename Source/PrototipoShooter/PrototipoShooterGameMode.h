// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PrototipoShooterGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class APrototipoShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	APrototipoShooterGameMode();

protected:

	/** Cria os alvos de teste ao iniciar o jogo */
	UPROPERTY(EditAnywhere, Category="Teste")
	bool bSpawnTestTargets = true;

	/** Distancia dos alvos ate o ponto de inicio do jogador */
	UPROPERTY(EditAnywhere, Category="Teste")
	float TargetDistance = 1200.f;

	virtual void BeginPlay() override;

	/** Coloca um alvo ao Norte, Sul, Leste e Oeste do ponto de inicio */
	void SpawnTestTargets();
};



