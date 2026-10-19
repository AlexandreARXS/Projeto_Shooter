// Primeiro inimigo: boneco parado que leva dano no corpo e na cabeca, morre e reaparece

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterEnemy.generated.h"

class UCapsuleComponent;
class USphereComponent;
class UAnimSequence;

UCLASS()
class AShooterEnemy : public ACharacter
{
	GENERATED_BODY()

	/** Area de acerto do corpo (do chao ate o pescoco) */
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UCapsuleComponent> BodyHitbox;

	/** Area de acerto da cabeca (presa ao osso "head", acompanha a animacao) */
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USphereComponent> HeadHitbox;

public:

	AShooterEnemy();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category="Inimigo")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category="Inimigo")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category="Inimigo")
	bool IsDead() const { return bIsDead; }

	/** Ponto acima da cabeca, onde a HUD desenha a barra de vida */
	FVector GetHealthBarLocation() const;

protected:

	/**
	 * Vida maxima. Com o dano da arma (20):
	 * corpo = 20 -> 5 tiros; cabeca = 20 x 2.5 = 50 -> 2 tiros
	 */
	UPROPERTY(EditAnywhere, Category="Inimigo")
	float MaxHealth = 100.f;

	/** Multiplicador de dano na cabeca */
	UPROPERTY(EditAnywhere, Category="Inimigo")
	float HeadshotMultiplier = 2.5f;

	/** Segundos ate o inimigo reaparecer depois de abatido (para testes) */
	UPROPERTY(EditAnywhere, Category="Inimigo")
	float RespawnDelay = 5.f;

	/** Pontos por acerto no corpo, na cabeca e por abate */
	UPROPERTY(EditAnywhere, Category="Pontos")
	int32 BodyHitPoints = 10;

	UPROPERTY(EditAnywhere, Category="Pontos")
	int32 HeadshotPoints = 25;

	UPROPERTY(EditAnywhere, Category="Pontos")
	int32 KillPoints = 100;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Inimigo")
	float Health = 100.f;

	bool bIsDead = false;

	/** Animacoes de morte (uma e escolhida ao acaso) */
	UPROPERTY()
	TArray<TObjectPtr<UAnimSequence>> DeathAnimations;

	/** Animation Blueprint usado enquanto vivo */
	UPROPERTY()
	TSubclassOf<UAnimInstance> AliveAnimClass;

	FTimerHandle RespawnTimer;

	void Die();
	void Respawn();
	void SetHitboxesEnabled(bool bEnabled);
};
