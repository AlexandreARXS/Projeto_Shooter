// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrototipoShooterCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputCoreTypes.h"
#include "PrototipoShooter.h"

APrototipoShooterCharacter::APrototipoShooterCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Camera sobre o ombro (os valores finais sao aplicados no BeginPlay)
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom->TargetArmLength = ShoulderArmLength;
	CameraBoom->SocketOffset = ShoulderOffset;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void APrototipoShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APrototipoShooterCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &APrototipoShooterCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APrototipoShooterCharacter::Look);

		// Acoes do shooter
		SetupShooterInput();
		EnhancedInputComponent->BindAction(ShoulderSwapAction, ETriggerEvent::Started, this, &APrototipoShooterCharacter::DoSwapShoulder);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &APrototipoShooterCharacter::DoAimStart);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &APrototipoShooterCharacter::DoAimEnd);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Canceled, this, &APrototipoShooterCharacter::DoAimEnd);
	}
	else
	{
		UE_LOG(LogPrototipoShooter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APrototipoShooterCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void APrototipoShooterCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void APrototipoShooterCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void APrototipoShooterCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void APrototipoShooterCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void APrototipoShooterCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void APrototipoShooterCharacter::SetupShooterInput()
{
	// cria a acao de troca de ombro se ela nao foi definida no Blueprint
	if (!ShoulderSwapAction)
	{
		ShoulderSwapAction = NewObject<UInputAction>(this, TEXT("IA_TrocarOmbro"));
	}

	// cria a acao de mira se ela nao foi definida no Blueprint
	if (!AimAction)
	{
		AimAction = NewObject<UInputAction>(this, TEXT("IA_Mirar"));
	}

	// cria o mapping context com as teclas do shooter
	if (!ShooterMappingContext)
	{
		ShooterMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Shooter"));
		ShooterMappingContext->MapKey(ShoulderSwapAction, EKeys::Q);
		ShooterMappingContext->MapKey(ShoulderSwapAction, EKeys::Gamepad_RightThumbstick);
		ShooterMappingContext->MapKey(AimAction, EKeys::RightMouseButton);
		ShooterMappingContext->MapKey(AimAction, EKeys::Gamepad_LeftTrigger);
	}

	// registra o mapping context no jogador local
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (!Subsystem->HasMappingContext(ShooterMappingContext))
			{
				Subsystem->AddMappingContext(ShooterMappingContext, 1);
			}
		}
	}
}

void APrototipoShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentShoulderSide = bRightShoulder ? 1.f : -1.f;

	// guarda os valores originais para restaurar ao sair da mira
	DefaultFOV = FollowCamera->FieldOfView;
	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultRotationRate = GetCharacterMovement()->RotationRate;

	CameraBoom->bEnableCameraLag = CameraLagSpeed > 0.f;
	CameraBoom->CameraLagSpeed = CameraLagSpeed;

	ApplyShoulderCamera();
}

void APrototipoShooterCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// desliza a camera suavemente para o ombro escolhido
	const float TargetSide = bRightShoulder ? 1.f : -1.f;
	if (!FMath::IsNearlyEqual(CurrentShoulderSide, TargetSide, 0.001f))
	{
		CurrentShoulderSide = FMath::FInterpTo(CurrentShoulderSide, TargetSide, DeltaSeconds, ShoulderSwapSpeed);
	}

	// aproxima/afasta a camera ao entrar/sair da mira
	AimAlpha = FMath::FInterpTo(AimAlpha, bIsAiming ? 1.f : 0.f, DeltaSeconds, AimInterpSpeed);

	ApplyShoulderCamera();
}

void APrototipoShooterCharacter::ApplyShoulderCamera()
{
	// mistura a camera normal com a camera de mira conforme o AimAlpha
	const float ArmLength = FMath::Lerp(ShoulderArmLength, AimArmLength, AimAlpha);
	const FVector Offset = FMath::Lerp(ShoulderOffset, AimOffset, AimAlpha);

	CameraBoom->TargetArmLength = ArmLength;
	CameraBoom->SocketOffset = FVector(Offset.X, Offset.Y * CurrentShoulderSide, Offset.Z);
	FollowCamera->SetFieldOfView(FMath::Lerp(DefaultFOV, AimFOV, AimAlpha));
}

void APrototipoShooterCharacter::DoAimStart()
{
	bIsAiming = true;

	// o personagem passa a encarar a direcao da camera, em vez da direcao do movimento
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = false;
	Movement->bUseControllerDesiredRotation = true;
	Movement->RotationRate = FRotator(0.f, AimRotationRate, 0.f);
	Movement->MaxWalkSpeed = AimWalkSpeed;
}

void APrototipoShooterCharacter::DoAimEnd()
{
	bIsAiming = false;

	// volta ao movimento livre
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->bUseControllerDesiredRotation = false;
	Movement->RotationRate = DefaultRotationRate;
	Movement->MaxWalkSpeed = DefaultWalkSpeed;
}

void APrototipoShooterCharacter::DoSwapShoulder()
{
	bRightShoulder = !bRightShoulder;
}
