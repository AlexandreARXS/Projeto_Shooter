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

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

