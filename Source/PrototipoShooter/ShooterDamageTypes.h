// Tipos de dano do shooter

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "ShooterDamageTypes.generated.h"

/** Dano de golpe corpo-a-corpo (nao conta como tiro na cabeca) */
UCLASS()
class UShooterMeleeDamageType : public UDamageType
{
	GENERATED_BODY()
};
