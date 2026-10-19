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
	FTimerHandle FireTimer;

	// ---------- Interacao ----------

	/** Objeto interativo ao alcance do jogador */
	TWeakObjectPtr<AShooterInteractable> FocusedInteractable;

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

	/** Soma pontos (chamado pelos alvos) */
	void AddScore(int32 Points);

	UFUNCTION(BlueprintPure, Category="Pontos")
	int32 GetScore() const { return Score; }

	/** Interage com o objeto ao alcance */
	UFUNCTION(BlueprintCallable, Category="Interacao")
	virtual void DoInteract();

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

