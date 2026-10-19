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

	/** Distancia da caixa de interacao atras do alvo Oeste */
	UPROPERTY(EditAnywhere, Category="Teste")
	float InteractableBehindDistance = 250.f;

	/** Posicao do inimigo em relacao ao ponto de inicio (X = Norte, Y = Leste) */
	UPROPERTY(EditAnywhere, Category="Teste")
	FVector EnemyOffset = FVector(700.f, 450.f, 0.f);

	virtual void BeginPlay() override;

	/** Coloca um alvo ao Norte, Sul, Leste e Oeste do ponto de inicio, e a caixa de interacao */
	void SpawnTestTargets();
};



