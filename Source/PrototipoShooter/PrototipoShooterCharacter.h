// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PrototipoShooterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class AShooterWeapon;
class AShooterInteractable;
class AShooterCover;
class UAnimSequence;
class UAnimInstance;
class UCapsuleComponent;
class USphereComponent;
class SPauseMenuWidget;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class APrototipoShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Area de acerto do corpo do jogador (tiros inimigos) */
	UPROPERTY(VisibleAnywhere, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> PlayerBodyHitbox;

	/** Area de acerto da cabeca do jogador (presa ao osso "head") */
	UPROPERTY(VisibleAnywhere, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> PlayerHeadHitbox;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	/** Troca de ombro. Se ficar vazio, e criado em codigo (tecla Q / clique do analogico direito) */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ShoulderSwapAction;

	/** Mira (segurar). Se ficar vazio, e criado em codigo (botao direito do mouse / gatilho esquerdo) */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> AimAction;

	/** Atirar (segurar = automatico). Se ficar vazio, e criado em codigo (botao esquerdo do mouse / gatilho direito) */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> FireAction;

	/** Golpe corpo-a-corpo. Se ficar vazio, e criado em codigo (tecla V / botao B do controle) */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MeleeAction;

	/** Pausar. Se ficar vazio, e criado em codigo (Esc, P / Start no controle) */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> PauseAction;

	/** Recarregar. Se ficar vazio, e criado em codigo (tecla R / botao RB do controle) */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ReloadAction;

	/** Interagir. Se ficar vazio, e criado em codigo (tecla E / botao X do controle) */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> InteractAction;

	/** Mapping context do shooter, criado em codigo */
	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> ShooterMappingContext;

	// ---------- Camera sobre o ombro ----------

	/** Distancia da camera ate o personagem */
	UPROPERTY(EditAnywhere, Category="Camera|Ombro")
	float ShoulderArmLength = 160.f;

	/** Deslocamento da camera: X = frente/tras, Y = lado (ombro direito positivo), Z = altura */
	UPROPERTY(EditAnywhere, Category="Camera|Ombro")
	FVector ShoulderOffset = FVector(0.f, 55.f, 65.f);

	/** Velocidade da transicao ao trocar de ombro */
	UPROPERTY(EditAnywhere, Category="Camera|Ombro")
	float ShoulderSwapSpeed = 10.f;

	/** Suaviza o acompanhamento da camera (0 desliga) */
	UPROPERTY(EditAnywhere, Category="Camera|Ombro")
	float CameraLagSpeed = 18.f;

	/** true = ombro direito, false = ombro esquerdo */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Camera|Ombro")
	bool bRightShoulder = true;

	/** Lado atual da camera, interpolado entre -1 (esquerdo) e 1 (direito) */
	float CurrentShoulderSide = 1.f;

	// ---------- Mira ----------

	/** Distancia da camera ao personagem enquanto mira */
	UPROPERTY(EditAnywhere, Category="Camera|Mira")
	float AimArmLength = 90.f;

	/** Deslocamento da camera enquanto mira (Y segue o ombro escolhido) */
	UPROPERTY(EditAnywhere, Category="Camera|Mira")
	FVector AimOffset = FVector(0.f, 50.f, 62.f);

	/** Campo de visao enquanto mira (menor = mais zoom) */
	UPROPERTY(EditAnywhere, Category="Camera|Mira")
	float AimFOV = 65.f;

	/** Velocidade da transicao ao entrar/sair da mira */
	UPROPERTY(EditAnywhere, Category="Camera|Mira")
	float AimInterpSpeed = 12.f;

	/** Velocidade de caminhada enquanto mira */
	UPROPERTY(EditAnywhere, Category="Camera|Mira")
	float AimWalkSpeed = 250.f;

	/** Velocidade com que o personagem gira para a direcao da camera enquanto mira */
	UPROPERTY(EditAnywhere, Category="Camera|Mira")
	float AimRotationRate = 720.f;

	/** true enquanto o botao de mira esta pressionado */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Camera|Mira")
	bool bIsAiming = false;

	/** 0 = sem mira, 1 = mira completa (interpolado) */
	float AimAlpha = 0.f;

	// PROVISORIO: Arma, poses de descanso/tiro por offset fixo (as maos nao seguram a arma). Trocar por animacoes de rifle + socket na mao.
	// ---------- Arma ----------

	/** Classe da arma criada no BeginPlay */
	UPROPERTY(EditAnywhere, Category="Arma")
	TSubclassOf<AShooterWeapon> WeaponClass;

	/** Arma atual */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Arma")
	TObjectPtr<AShooterWeapon> Weapon;

	/** Posicao de descanso, relativa ao personagem: X = frente, Y = direita, Z = altura (0 = quadril) */
	UPROPERTY(EditAnywhere, Category="Arma")
	FVector WeaponRestOffset = FVector(28.f, 24.f, -8.f);

	/** Rotacao de descanso (Pitch negativo = cano apontando para baixo) */
	UPROPERTY(EditAnywhere, Category="Arma")
	FRotator WeaponRestRotation = FRotator(-40.f, -10.f, 0.f);

	/** Posicao de tiro, relativa ao personagem (altura do ombro direito) */
	UPROPERTY(EditAnywhere, Category="Arma")
	FVector WeaponAimOffset = FVector(38.f, 20.f, 48.f);

	/** Por quanto tempo a arma continua levantada depois do ultimo tiro */
	UPROPERTY(EditAnywhere, Category="Arma")
	float CombatStanceDuration = 1.f;

	/** Velocidade da transicao entre descanso e tiro */
	UPROPERTY(EditAnywhere, Category="Arma")
	float CombatInterpSpeed = 14.f;

	// ---------- Tiro ----------

	/** Tempo entre tiros (0.12 = ~8 tiros por segundo) */
	UPROPERTY(EditAnywhere, Category="Tiro")
	float FireInterval = 0.12f;

	/** Dano por tiro */
	UPROPERTY(EditAnywhere, Category="Tiro")
	float ShotDamage = 20.f;

	/** Alcance maximo do tiro */
	UPROPERTY(EditAnywhere, Category="Tiro")
	float FireRange = 10000.f;

	/** Imprecisao sem mirar, em graus */
	UPROPERTY(EditAnywhere, Category="Tiro")
	float HipFireSpread = 2.5f;

	/** Imprecisao mirando, em graus */
	UPROPERTY(EditAnywhere, Category="Tiro")
	float AimFireSpread = 0.4f;

	// ---------- Municao ----------

	/** Balas por pente */
	UPROPERTY(EditAnywhere, Category="Tiro")
	int32 MagazineSize = 40;

	/** Tempo de recarga, em segundos (nao da para atirar durante a recarga) */
	UPROPERTY(EditAnywhere, Category="Tiro")
	float ReloadTime = 2.5f;

	/** Balas no pente agora */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Tiro")
	int32 CurrentAmmo = 40;

	bool bIsReloading = false;
	float ReloadStartTime = -1.f;
	FTimerHandle ReloadTimer;

	// ---------- Corpo-a-corpo ----------

	/** Dano do golpe (100 de vida no inimigo = 2 golpes) */
	UPROPERTY(EditAnywhere, Category="Corpo-a-corpo")
	float MeleeDamage = 50.f;

	/** Alcance do golpe a partir da frente do personagem */
	UPROPERTY(EditAnywhere, Category="Corpo-a-corpo")
	float MeleeRange = 140.f;

	/** Largura do golpe (raio da esfera que varre a frente) */
	UPROPERTY(EditAnywhere, Category="Corpo-a-corpo")
	float MeleeRadius = 45.f;

	/** Altura do golpe em relacao ao centro do personagem (peito) */
	UPROPERTY(EditAnywhere, Category="Corpo-a-corpo")
	float MeleeHeight = 40.f;

	/** Momento do impacto depois de apertar o botao */
	UPROPERTY(EditAnywhere, Category="Corpo-a-corpo")
	float MeleeHitDelay = 0.25f;

	/** Tempo minimo entre golpes */
	UPROPERTY(EditAnywhere, Category="Corpo-a-corpo")
	float MeleeCooldown = 0.9f;

	/** Duracao maxima do golpe (o personagem fica parado durante ele) */
	UPROPERTY(EditAnywhere, Category="Corpo-a-corpo")
	float MeleeMaxDuration = 1.f;

	// PROVISORIO: Corpo-a-corpo, animacao do soco desarmado do template (MM_Attack_01). Trocar por golpe com a arma.
	/** Animacao do golpe */
	UPROPERTY(EditAnywhere, Category="Corpo-a-corpo")
	TObjectPtr<UAnimSequence> MeleeAnimation;

	bool bIsMeleeing = false;
	float LastMeleeTime = -100.f;
	FTimerHandle MeleeHitTimer;
	FTimerHandle MeleeEndTimer;

	/** Animation Blueprint normal, restaurado depois do golpe */
	UPROPERTY()
	TSubclassOf<UAnimInstance> DefaultAnimClass;

	/** Momento do impacto: procura quem esta na frente e aplica o dano */
	void PerformMeleeHit();

	/** Fim do golpe: volta a animacao e o movimento */
	void EndMelee();

	// ---------- Cobertura ----------

	/** Meia altura da capsula agachado (em pe = 96) */
	UPROPERTY(EditAnywhere, Category="Cobertura")
	float CrouchedCapsuleHalfHeight = 60.f;

	/** Velocidade andando agachado atras da cobertura */
	UPROPERTY(EditAnywhere, Category="Cobertura")
	float CoverCrouchSpeed = 200.f;

	// PROVISORIO: Agachamento, animacao provisoria (quadro de MM_Land). Trocar por animacao de agachar real (ex.: Lyra).
	/**
	 * Pose agachada PROVISORIA: o template nao tem animacao de agachar,
	 * entao usamos um quadro com os joelhos dobrados da animacao de aterrissagem
	 */
	UPROPERTY(EditAnywhere, Category="Cobertura")
	TObjectPtr<UAnimSequence> CoverPoseAnimation;

	/** Em que momento (segundos) da animacao esta a pose agachada */
	UPROPERTY(EditAnywhere, Category="Cobertura")
	float CoverPoseTime = 0.15f;

	/** Cobertura em que o jogador esta (ou nenhuma) */
	TWeakObjectPtr<AShooterCover> CurrentCover;

	bool bCoverPoseActive = false;

	/** Agacha/levanta conforme a cobertura e a mira */
	void UpdateCover();

	/** Volta para a animacao normal (Animation Blueprint) */
	void RestoreDefaultAnimation();

	// ---------- Vida ----------

	/**
	 * Vida maxima. Mesma regra do inimigo, com a arma inimiga (20 de dano):
	 * corpo = 20 -> 5 tiros; cabeca = 20 x 2.5 = 50 -> 2 tiros
	 */
	UPROPERTY(EditAnywhere, Category="Vida")
	float MaxHealth = 100.f;

	/** Multiplicador de dano na cabeca */
	UPROPERTY(EditAnywhere, Category="Vida")
	float HeadshotMultiplier = 2.5f;

	/** Segundos entre a morte e o reinicio do mapa */
	UPROPERTY(EditAnywhere, Category="Vida")
	float RestartDelay = 4.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Vida")
	float Health = 100.f;

	bool bIsDead = false;
	float LastDamageTime = -100.f;
	bool bLastDamageWasHeadshot = false;
	FTimerHandle RestartTimer;

	/** Animacao de morte do jogador */
	UPROPERTY(EditAnywhere, Category="Vida")
	TObjectPtr<UAnimSequence> DeathAnimation;

	/** Morre: para tudo, toca a animacao e agenda o reinicio */
	void Die();

	/** Recarrega o mapa atual (recomeca a fase) */
	void RestartLevel();

	/** Liga/desliga as areas de acerto do jogador */
	void SetHitboxesEnabled(bool bEnabled);

	// ---------- Pausa ----------

	/** Mapa do menu principal (botao MENU PRINCIPAL da pausa) */
	UPROPERTY(EditAnywhere, Category="Pausa")
	FName MainMenuLevel = TEXT("/Game/Maps/MainMenu_Principal");

	TSharedPtr<SPauseMenuWidget> PauseWidget;

	void ResumeGame();
	void GoToMainMenu();
	void QuitFromPause();

	/** Tira o menu de pausa da tela */
	void ClosePauseMenu();

	/** Pontuacao total do jogador */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Pontos")
	int32 Score = 0;

	/** Termina a recarga e enche o pente */
	void FinishReload();

	bool bFireHeld = false;
	bool bInCombatStance = false;

	/** 0 = arma em descanso, 1 = arma em posicao de tiro (interpolado) */
	float CombatAlpha = 0.f;

	float LastShotTime = -100.f;
	float LastHitConfirmTime = -100.f;
	bool bLastHitWasHeadshot = false;
	float LastKillTime = -100.f;
	FTimerHandle FireTimer;

	// ---------- Interacao ----------

	/** Objeto interativo ao alcance do jogador */
	TWeakObjectPtr<AShooterInteractable> FocusedInteractable;

	/** Segurando o botao de interacao (objetos que exigem segurar) */
	bool bHoldingInteract = false;
	float InteractHoldStart = 0.f;

	/** Avanca a interacao de segurar e conclui quando o tempo completa */
	void UpdateInteractHold();

	// ---------- Colecionaveis ----------

	/** Nomes dos colecionaveis ja pegos */
	TArray<FString> CollectedItems;
	float LastCollectTime = -100.f;

	/** Mensagem atual na tela e ate quando ela fica visivel */
	FString ScreenMessage;
	float ScreenMessageEndTime = -1.f;

	/** Valores originais, guardados no BeginPlay para restaurar ao sair da mira */
	float DefaultFOV = 90.f;
	float DefaultWalkSpeed = 500.f;
	FRotator DefaultRotationRate = FRotator(0.f, 500.f, 0.f);

public:

	/** Constructor */
	APrototipoShooterCharacter();	

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Cria (se preciso) e registra as acoes extras do shooter */
	void SetupShooterInput();

	/** Aplica a posicao da camera conforme o ombro atual */
	void ApplyShoulderCamera();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Agachar/levantar: ajusta as areas de acerto (agachado na cobertura, so a parte de cima fica exposta) */
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	/** Liga/desliga a postura de combate (personagem encara a camera) */
	void UpdateCombatStance();

	/** Posiciona a arma entre descanso e tiro */
	void UpdateWeaponTransform();

	/** Dispara um tiro */
	void FireShot();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	/** Troca a camera de ombro */
	UFUNCTION(BlueprintCallable, Category="Camera")
	virtual void DoSwapShoulder();

	/** Comeca a mirar */
	UFUNCTION(BlueprintCallable, Category="Mira")
	virtual void DoAimStart();

	/** Para de mirar */
	UFUNCTION(BlueprintCallable, Category="Mira")
	virtual void DoAimEnd();

	/** Esta mirando? */
	UFUNCTION(BlueprintPure, Category="Mira")
	bool IsAiming() const { return bIsAiming; }

	/** Quanto da mira esta aplicado (0 a 1), usado pela HUD */
	UFUNCTION(BlueprintPure, Category="Mira")
	float GetAimAlpha() const { return AimAlpha; }

	/** Comeca a atirar */
	UFUNCTION(BlueprintCallable, Category="Tiro")
	virtual void DoFireStart();

	/** Para de atirar */
	UFUNCTION(BlueprintCallable, Category="Tiro")
	virtual void DoFireEnd();

	/** Quanto a arma esta levantada (0 a 1), usado pela HUD */
	UFUNCTION(BlueprintPure, Category="Tiro")
	float GetCombatAlpha() const { return CombatAlpha; }

	/** Leva dano (tiros inimigos): corpo ou cabeca */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/** Abre o menu de pausa */
	UFUNCTION(BlueprintCallable, Category="Pausa")
	virtual void DoPause();

	UFUNCTION(BlueprintPure, Category="Vida")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category="Vida")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category="Vida")
	bool IsDead() const { return bIsDead; }

	/** Momento do ultimo dano, usado pela HUD (tela vermelha) */
	float GetLastDamageTime() const { return LastDamageTime; }

	/** Ponto do peito, onde os inimigos miram */
	FVector GetAimTargetLocation() const;

	/** Posicao da cabeca (osso "head") */
	FVector GetHeadLocation() const;

	/** Chamado pela cobertura quando o jogador entra/sai da area dela */
	void EnterCover(AShooterCover* Cover);
	void LeaveCover(AShooterCover* Cover);

	/** Esta em uma area de cobertura? usado pela HUD */
	bool IsInCover() const;

	/** Golpe corpo-a-corpo */
	UFUNCTION(BlueprintCallable, Category="Corpo-a-corpo")
	virtual void DoMelee();

	UFUNCTION(BlueprintPure, Category="Corpo-a-corpo")
	bool IsMeleeing() const { return bIsMeleeing; }

	/** Recarrega (se o pente nao estiver cheio) */
	UFUNCTION(BlueprintCallable, Category="Tiro")
	virtual void DoReload();

	UFUNCTION(BlueprintPure, Category="Tiro")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	UFUNCTION(BlueprintPure, Category="Tiro")
	int32 GetMagazineSize() const { return MagazineSize; }

	UFUNCTION(BlueprintPure, Category="Tiro")
	bool IsReloading() const { return bIsReloading; }

	/** Progresso da recarga (0 a 1), usado pela HUD */
	UFUNCTION(BlueprintPure, Category="Tiro")
	float GetReloadProgress() const;

	/** Soma pontos (chamado pelos alvos e inimigos) */
	void AddScore(int32 Points);

	/** Chamado pelo inimigo atingido (muda o marcador de acerto e avisa o abate) */
	void NotifyEnemyHit(bool bHeadshot, bool bKilled, bool bMelee = false);

	/** O ultimo acerto foi na cabeca? usado pela HUD */
	bool WasLastHitHeadshot() const { return bLastHitWasHeadshot; }

	/** Momento do ultimo abate, usado pela HUD */
	float GetLastKillTime() const { return LastKillTime; }

	UFUNCTION(BlueprintPure, Category="Pontos")
	int32 GetScore() const { return Score; }

	/** Interage com o objeto ao alcance (ou comeca a segurar) */
	UFUNCTION(BlueprintCallable, Category="Interacao")
	virtual void DoInteract();

	/** Soltou o botao de interacao */
	UFUNCTION(BlueprintCallable, Category="Interacao")
	virtual void DoInteractRelease();

	/** Progresso da interacao de segurar (0 a 1), usado pela HUD */
	float GetInteractHoldProgress() const;

	/** Registra um colecionavel pego (chamado pelo colecionavel) */
	void AddCollectible(const FString& ItemName);

	int32 GetCollectedCount() const { return CollectedItems.Num(); }
	float GetLastCollectTime() const { return LastCollectTime; }
	FString GetLastCollectedName() const { return CollectedItems.Num() > 0 ? CollectedItems.Last() : FString(); }

	/** Chamado pelo objeto interativo quando o jogador entra/sai da area dele */
	void SetFocusedInteractable(AShooterInteractable* Interactable);
	void ClearFocusedInteractable(AShooterInteractable* Interactable);

	/** Objeto interativo ao alcance (ou nullptr), usado pela HUD */
	AShooterInteractable* GetFocusedInteractable() const;

	/** Mostra uma mensagem no centro da tela por alguns segundos */
	void ShowScreenMessage(const FString& Message, float Duration);

	/** Retorna true e a mensagem se houver uma mensagem ativa, usado pela HUD */
	bool GetActiveScreenMessage(FString& OutMessage) const;

	/** Momento do ultimo acerto confirmado, usado pela HUD */
	UFUNCTION(BlueprintPure, Category="Tiro")
	float GetLastHitConfirmTime() const { return LastHitConfirmTime; }

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

