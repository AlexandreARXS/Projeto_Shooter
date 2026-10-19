// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrototipoShooterGameMode.h"
#include "ShooterHUD.h"
#include "ShooterTarget.h"
#include "ShooterInteractable.h"
#include "ShooterEnemy.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"

APrototipoShooterGameMode::APrototipoShooterGameMode()
{
	// HUD com a mira na tela
	HUDClass = AShooterHUD::StaticClass();
}

namespace
{
	/** Altura do chao embaixo de um ponto (ou um pouco abaixo do centro, se nao achar chao) */
	float FindGroundZ(UWorld* World, const FVector& Location, const FVector& Center)
	{
		FHitResult Ground;
		const FVector TraceStart(Location.X, Location.Y, Center.Z + 300.f);
		const FVector TraceEnd(Location.X, Location.Y, Center.Z - 3000.f);
		if (World->LineTraceSingleByChannel(Ground, TraceStart, TraceEnd, ECC_Visibility))
		{
			return Ground.ImpactPoint.Z;
		}
		return Center.Z - 90.f;
	}
}

void APrototipoShooterGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (bSpawnTestTargets)
	{
		SpawnTestTargets();
	}
}

void APrototipoShooterGameMode::SpawnTestTargets()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// centro = ponto de inicio do jogador
	FVector Center = FVector::ZeroVector;
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		Center = It->GetActorLocation();
		break;
	}

	// Norte = +X, Sul = -X, Leste = +Y, Oeste = -Y (eixos do mapa)
	struct FTargetSpot
	{
		const TCHAR* Name;
		FVector Direction;
	};

	const FTargetSpot Spots[] =
	{
		{ TEXT("Norte"), FVector(1.f, 0.f, 0.f) },
		{ TEXT("Sul"),   FVector(-1.f, 0.f, 0.f) },
		{ TEXT("Leste"), FVector(0.f, 1.f, 0.f) },
		{ TEXT("Oeste"), FVector(0.f, -1.f, 0.f) },
	};

	for (const FTargetSpot& Spot : Spots)
	{
		FVector Location = Center + Spot.Direction * TargetDistance;
		Location.Z = FindGroundZ(World, Location, Center);

		// o alvo fica de frente para o centro
		const FRotator Facing(0.f, (-Spot.Direction).Rotation().Yaw, 0.f);

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (AShooterTarget* Target = World->SpawnActor<AShooterTarget>(AShooterTarget::StaticClass(), Location, Facing, Params))
		{
			Target->TargetName = Spot.Name;
		}
	}

	// ponto de interacao logo atras do alvo Oeste
	FVector BoxLocation = Center + FVector(0.f, -1.f, 0.f) * (TargetDistance + InteractableBehindDistance);
	BoxLocation.Z = FindGroundZ(World, BoxLocation, Center);

	FActorSpawnParameters BoxParams;
	BoxParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (AShooterInteractable* Box = World->SpawnActor<AShooterInteractable>(AShooterInteractable::StaticClass(), BoxLocation, FRotator::ZeroRotator, BoxParams))
	{
		// "Voce saira daqui", com acentos
		Box->Message = FString(TEXT("Voc\u00EA sair\u00E1 daqui"));
	}

	// primeiro inimigo: entre o centro e os alvos Norte e Leste, virado para o jogador
	FVector EnemyLocation = Center + EnemyOffset;
	EnemyLocation.Z = FindGroundZ(World, EnemyLocation, Center) + 98.f;
	const FRotator EnemyFacing(0.f, (Center - EnemyLocation).Rotation().Yaw, 0.f);

	FActorSpawnParameters EnemyParams;
	EnemyParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	World->SpawnActor<AShooterEnemy>(AShooterEnemy::StaticClass(), EnemyLocation, EnemyFacing, EnemyParams);
}
