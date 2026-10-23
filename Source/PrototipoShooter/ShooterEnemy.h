// Primeiro inimigo: patrulha, ve o jogador e atira em rajadas; leva dano no corpo e na cabeca, morre e reaparece

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterEnemy.generated.h"

class UCapsuleComponent;
class USphereComponent;
class UAnimSequence;
class AShooterWeapon;
class APrototipoShooterCharacter;

// PROVISORIO: Inimigo, IA simples (patrulha em linha reta sem NavMesh, da meia-volta em obstaculos; ao ver o jogador para e atira parado, sem buscar cobertura nem perseguir). Trocar por StateTree/Behavior Tree + NavMesh.
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
	virtual void Tick(float DeltaSeconds) override;
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

	/** Viu o jogador e esta em combate? usado pela HUD ("!") */
	UFUNCTION(BlueprintPure, Category="Inimigo")
	bool IsEngaged() const { return bEngaged; }

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

	// PROVISORIO: Inimigo, reaparece sozinho para facilitar os testes. Remover quando houver ondas/spawners.
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

	// ---------- Patrulha ----------

	/** Distancia da patrulha, para a direita de onde o inimigo nasce */
	UPROPERTY(EditAnywhere, Category="Patrulha")
	float PatrolDistance = 600.f;

	/** Velocidade andando na patrulha */
	UPROPERTY(EditAnywhere, Category="Patrulha")
	float PatrolSpeed = 160.f;

	/** Tempo parado em cada ponta da patrulha */
	UPROPERTY(EditAnywhere, Category="Patrulha")
	float PatrolWaitTime = 1.5f;

	/** Tempo parado depois de levar um tiro */
	UPROPERTY(EditAnywhere, Category="Patrulha")
	float HitPauseTime = 0.6f;

	/** Distancia a frente em que o inimigo procura obstaculos */
	UPROPERTY(EditAnywhere, Category="Patrulha")
	float ObstacleProbeDistance = 80.f;

	/** Tempo parado antes de voltar quando encontra um obstaculo */
	UPROPERTY(EditAnywhere, Category="Patrulha")
	float ObstacleWaitTime = 0.5f;

	/** A cada quantos segundos confere se o inimigo ficou travado */
	UPROPERTY(EditAnywhere, Category="Patrulha")
	float StuckCheckInterval = 0.4f;

	/** Se andar menos que isso entre duas conferencias, considera travado */
	UPROPERTY(EditAnywhere, Category="Patrulha")
	float StuckMinDistance = 15.f;

	FVector StuckCheckLocation = FVector::ZeroVector;
	float StuckCheckTime = 0.f;

	FVector PatrolStart = FVector::ZeroVector;
	FVector PatrolEnd = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	bool bMovingToEnd = true;
	float WaitUntil = 0.f;

	// ---------- Visao ----------

	/** Distancia maxima em que o inimigo enxerga o jogador */
	UPROPERTY(EditAnywhere, Category="Visao")
	float SightRange = 2500.f;

	/** Metade do campo de visao, em graus (60 = cone de 120 graus a frente) */
	UPROPERTY(EditAnywhere, Category="Visao")
	float SightHalfAngle = 60.f;

	/** Tempo entre ver o jogador e o primeiro tiro */
	UPROPERTY(EditAnywhere, Category="Visao")
	float ReactionTime = 0.6f;

	/** Segundos sem ver o jogador ate desistir e voltar a patrulha */
	UPROPERTY(EditAnywhere, Category="Visao")
	float LoseSightTime = 2.f;

	// ---------- Tiro (rajadas) ----------

	/** Tiros por rajada */
	UPROPERTY(EditAnywhere, Category="Tiro")
	int32 BurstSize = 3;

	/** Tempo entre os tiros de uma mesma rajada */
	UPROPERTY(EditAnywhere, Category="Tiro")
	float BurstShotInterval = 0.1f;

	/** Pausa entre uma rajada e a proxima */
	UPROPERTY(EditAnywhere, Category="Tiro")
	float BurstPause = 1.5f;

	/** Dano por tiro (mesma arma do jogador: 5 no corpo / 2 na cabeca) */
	UPROPERTY(EditAnywhere, Category="Tiro")
	float ShotDamage = 20.f;

	/** Imprecisao do inimigo, em graus */
	UPROPERTY(EditAnywhere, Category="Tiro")
	float AimSpreadDegrees = 2.5f;

	/** Alcance do tiro */
	UPROPERTY(EditAnywhere, Category="Tiro")
	float FireRange = 5000.f;

	/** So atira quando estiver virado para o jogador (graus) */
	UPROPERTY(EditAnywhere, Category="Tiro")
	float FireFacingTolerance = 20.f;

	/** Posicoes da arma: descanso (patrulha) e tiro (combate), relativas ao inimigo */
	UPROPERTY(EditAnywhere, Category="Tiro")
	FVector WeaponRestOffset = FVector(28.f, 24.f, -8.f);

	UPROPERTY(EditAnywhere, Category="Tiro")
	FRotator WeaponRestRotation = FRotator(-40.f, -10.f, 0.f);

	UPROPERTY(EditAnywhere, Category="Tiro")
	FVector WeaponAimOffset = FVector(38.f, 20.f, 48.f);

	/** Arma do inimigo (mesmo modelo provisorio do jogador) */
	UPROPERTY()
	TObjectPtr<AShooterWeapon> Weapon;

	TWeakObjectPtr<APrototipoShooterCharacter> TargetPlayer;
	bool bEngaged = false;
	bool bCanSeeTarget = false;
	float LastSeenTime = -100.f;
	float NextShotTime = 0.f;
	int32 ShotsLeftInBurst = 3;
	float CombatAlpha = 0.f;

	/** Procura o jogador no campo de visao */
	void UpdatePerception(float Now);

	/** Linha de visao livre ate a cabeca ou o peito do jogador? */
	bool CanSeePlayer(const APrototipoShooterCharacter* Player, bool bIgnoreAngle) const;

	/** Em combate: vira para o jogador e atira em rajadas */
	void UpdateCombat(float Now, float DeltaSeconds);

	/** Entra em combate com o jogador */
	void Engage(APrototipoShooterCharacter* Player, float Now);

	/** Sai do combate e volta a patrulhar */
	void Disengage(float Now);

	/** Um tiro da rajada */
	void FireAtTarget();

	/** Posiciona a arma entre descanso e tiro */
	void UpdateWeaponTransform();

	/** Ponto de onde o inimigo "olha" (cabeca) */
	FVector GetEyeLocation() const;

	/** Anda em direcao ao ponto atual da patrulha */
	void UpdatePatrol();

	/** Tem algo bloqueando o caminho logo a frente? */
	bool IsPathBlocked(const FVector& Direction) const;

	/** Andou quase nada desde a ultima conferencia? */
	bool IsStuck(float Now);

	/** Para e volta pelo caminho de onde veio */
	void TurnAround(float Now, float WaitTime);

	/** Reinicia a conferencia de travamento a partir da posicao atual */
	void ResetStuckCheck(float Now);

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
