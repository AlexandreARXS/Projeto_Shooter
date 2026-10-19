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
#include "ShooterWeapon.h"
#include "ShooterInteractable.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Engine/World.h"
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

	// arma padrao
	WeaponClass = AShooterWeapon::StaticClass();

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
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &APrototipoShooterCharacter::DoFireStart);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &APrototipoShooterCharacter::DoFireEnd);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &APrototipoShooterCharacter::DoFireEnd);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APrototipoShooterCharacter::DoInteract);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &APrototipoShooterCharacter::DoReload);
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

	// cria a acao de tiro se ela nao foi definida no Blueprint
	if (!FireAction)
	{
		FireAction = NewObject<UInputAction>(this, TEXT("IA_Atirar"));
	}

	// cria a acao de recarga se ela nao foi definida no Blueprint
	if (!ReloadAction)
	{
		ReloadAction = NewObject<UInputAction>(this, TEXT("IA_Recarregar"));
	}

	// cria a acao de interacao se ela nao foi definida no Blueprint
	if (!InteractAction)
	{
		InteractAction = NewObject<UInputAction>(this, TEXT("IA_Interagir"));
	}

	// cria o mapping context com as teclas do shooter
	if (!ShooterMappingContext)
	{
		ShooterMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Shooter"));
		ShooterMappingContext->MapKey(ShoulderSwapAction, EKeys::Q);
		ShooterMappingContext->MapKey(ShoulderSwapAction, EKeys::Gamepad_RightThumbstick);
		ShooterMappingContext->MapKey(AimAction, EKeys::RightMouseButton);
		ShooterMappingContext->MapKey(AimAction, EKeys::Gamepad_LeftTrigger);
		ShooterMappingContext->MapKey(FireAction, EKeys::LeftMouseButton);
		ShooterMappingContext->MapKey(FireAction, EKeys::Gamepad_RightTrigger);
		ShooterMappingContext->MapKey(InteractAction, EKeys::E);
		ShooterMappingContext->MapKey(ReloadAction, EKeys::R);
		ShooterMappingContext->MapKey(ReloadAction, EKeys::Gamepad_RightShoulder);
		ShooterMappingContext->MapKey(InteractAction, EKeys::Gamepad_FaceButton_Left);
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

	// comeca com o pente cheio
	CurrentAmmo = MagazineSize;

	// cria a arma
	if (!WeaponClass)
	{
		WeaponClass = AShooterWeapon::StaticClass();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Weapon = GetWorld()->SpawnActor<AShooterWeapon>(WeaponClass, GetActorTransform(), SpawnParams);
	UpdateWeaponTransform();

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

	// levanta/abaixa a arma
	UpdateCombatStance();
	CombatAlpha = FMath::FInterpTo(CombatAlpha, bInCombatStance ? 1.f : 0.f, DeltaSeconds, CombatInterpSpeed);

	ApplyShoulderCamera();
	UpdateWeaponTransform();
}

void APrototipoShooterCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(FireTimer);
	GetWorldTimerManager().ClearTimer(ReloadTimer);

	if (Weapon)
	{
		Weapon->Destroy();
		Weapon = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void APrototipoShooterCharacter::UpdateCombatStance()
{
	// em combate: mirando, segurando o gatilho ou logo depois de um tiro
	const float Now = GetWorld()->GetTimeSeconds();
	const bool bWantsCombat = bIsAiming || bFireHeld || (Now - LastShotTime < CombatStanceDuration);

	if (bWantsCombat == bInCombatStance)
	{
		return;
	}

	bInCombatStance = bWantsCombat;

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (bInCombatStance)
	{
		// o personagem passa a encarar a direcao da camera
		Movement->bOrientRotationToMovement = false;
		Movement->bUseControllerDesiredRotation = true;
		Movement->RotationRate = FRotator(0.f, AimRotationRate, 0.f);
	}
	else
	{
		// volta ao movimento livre
		Movement->bOrientRotationToMovement = true;
		Movement->bUseControllerDesiredRotation = false;
		Movement->RotationRate = DefaultRotationRate;
	}
}

void APrototipoShooterCharacter::UpdateWeaponTransform()
{
	if (!Weapon)
	{
		return;
	}

	const FTransform ActorTransform = GetActorTransform();

	// descanso: na cintura, cano para baixo
	const FVector RestLocation = ActorTransform.TransformPosition(WeaponRestOffset);
	const FQuat RestRotation = ActorTransform.TransformRotation(WeaponRestRotation.Quaternion());

	// tiro: no ombro, cano apontando para onde a camera mira
	const FVector AimLocation = ActorTransform.TransformPosition(WeaponAimOffset);
	const FVector AimTarget = FollowCamera->GetComponentLocation() + FollowCamera->GetForwardVector() * 5000.f;
	const FQuat AimRotation = (AimTarget - AimLocation).Rotation().Quaternion();

	const FVector Location = FMath::Lerp(RestLocation, AimLocation, CombatAlpha);
	const FQuat Rotation = FQuat::Slerp(RestRotation, AimRotation, CombatAlpha);

	Weapon->SetActorLocationAndRotation(Location, Rotation);
}

void APrototipoShooterCharacter::DoFireStart()
{
	bFireHeld = true;

	// recarregando: o tiro volta sozinho quando a recarga terminar, se o gatilho continuar apertado
	if (bIsReloading)
	{
		return;
	}

	// pente vazio: recarrega
	if (CurrentAmmo <= 0)
	{
		DoReload();
		return;
	}

	// respeita a cadencia mesmo clicando rapido
	const float Now = GetWorld()->GetTimeSeconds();
	const float Wait = FMath::Max(0.f, LastShotTime + FireInterval - Now);

	if (Wait <= 0.f)
	{
		FireShot();
		GetWorldTimerManager().SetTimer(FireTimer, this, &APrototipoShooterCharacter::FireShot, FireInterval, true);
	}
	else
	{
		GetWorldTimerManager().SetTimer(FireTimer, this, &APrototipoShooterCharacter::FireShot, FireInterval, true, Wait);
	}
}

void APrototipoShooterCharacter::DoFireEnd()
{
	bFireHeld = false;
	GetWorldTimerManager().ClearTimer(FireTimer);
}

void APrototipoShooterCharacter::FireShot()
{
	UWorld* World = GetWorld();
	if (!World || !Weapon)
	{
		return;
	}

	// sem tiro durante a recarga ou com o pente vazio
	if (bIsReloading || CurrentAmmo <= 0)
	{
		GetWorldTimerManager().ClearTimer(FireTimer);
		DoReload();
		return;
	}

	LastShotTime = World->GetTimeSeconds();
	CurrentAmmo--;

	// 1) linha a partir da camera, passando pelo centro da tela (com imprecisao)
	const float SpreadDegrees = FMath::Lerp(HipFireSpread, AimFireSpread, AimAlpha);
	const FVector CameraLocation = FollowCamera->GetComponentLocation();
	const FVector CameraDirection = FMath::VRandCone(FollowCamera->GetForwardVector(), FMath::DegreesToRadians(SpreadDegrees));

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ShooterFire), true);
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Weapon);

	FHitResult CameraHit;
	const FVector CameraEnd = CameraLocation + CameraDirection * FireRange;
	const bool bCameraHit = World->LineTraceSingleByChannel(CameraHit, CameraLocation, CameraEnd, ECC_Visibility, Params);
	const FVector AimPoint = bCameraHit ? CameraHit.ImpactPoint : CameraEnd;

	// 2) linha a partir do cano da arma ate o ponto mirado (respeita obstaculos na frente da arma)
	const FVector Muzzle = Weapon->GetMuzzleLocation();
	const FVector ShotDirection = (AimPoint - Muzzle).GetSafeNormal();

	FHitResult Hit;
	const bool bHit = World->LineTraceSingleByChannel(Hit, Muzzle, AimPoint + ShotDirection * 10.f, ECC_Visibility, Params);
	const FVector EndPoint = bHit ? Hit.ImpactPoint : AimPoint;

	// efeitos provisorios: rastro do tiro, clarao no cano e marca no impacto
	DrawDebugLine(World, Muzzle, EndPoint, FColor(255, 220, 80), false, 0.06f, 0, 1.5f);
	DrawDebugSphere(World, Muzzle, 5.f, 6, FColor::Orange, false, 0.04f);

	if (bHit)
	{
		DrawDebugPoint(World, Hit.ImpactPoint, 10.f, FColor::Red, false, 1.f);

		// aplica dano em quem foi atingido
		if (AActor* HitActor = Hit.GetActor())
		{
			const float Applied = UGameplayStatics::ApplyPointDamage(HitActor, ShotDamage, ShotDirection, Hit, GetController(), this, UDamageType::StaticClass());
			if (Applied > 0.f)
			{
				LastHitConfirmTime = LastShotTime;
			}
		}
	}

	// acabou o pente: para de atirar e recarrega sozinho
	if (CurrentAmmo <= 0)
	{
		GetWorldTimerManager().ClearTimer(FireTimer);
		DoReload();
	}
}

void APrototipoShooterCharacter::DoReload()
{
	// ja recarregando ou pente cheio
	if (bIsReloading || CurrentAmmo >= MagazineSize)
	{
		return;
	}

	bIsReloading = true;
	ReloadStartTime = GetWorld()->GetTimeSeconds();

	GetWorldTimerManager().ClearTimer(FireTimer);
	GetWorldTimerManager().SetTimer(ReloadTimer, this, &APrototipoShooterCharacter::FinishReload, ReloadTime, false);
}

void APrototipoShooterCharacter::FinishReload()
{
	bIsReloading = false;
	CurrentAmmo = MagazineSize;

	// se o gatilho continua apertado, volta a atirar
	if (bFireHeld)
	{
		FireShot();
		GetWorldTimerManager().SetTimer(FireTimer, this, &APrototipoShooterCharacter::FireShot, FireInterval, true);
	}
}

float APrototipoShooterCharacter::GetReloadProgress() const
{
	if (!bIsReloading || ReloadTime <= 0.f)
	{
		return 0.f;
	}

	return FMath::Clamp((GetWorld()->GetTimeSeconds() - ReloadStartTime) / ReloadTime, 0.f, 1.f);
}

void APrototipoShooterCharacter::AddScore(int32 Points)
{
	Score += Points;
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
	// a rotacao do personagem e tratada em UpdateCombatStance
	bIsAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;
}

void APrototipoShooterCharacter::DoAimEnd()
{
	bIsAiming = false;
	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
}

void APrototipoShooterCharacter::DoSwapShoulder()
{
	bRightShoulder = !bRightShoulder;
}

void APrototipoShooterCharacter::DoInteract()
{
	if (AShooterInteractable* Interactable = FocusedInteractable.Get())
	{
		Interactable->Interact(this);
	}
}

void APrototipoShooterCharacter::SetFocusedInteractable(AShooterInteractable* Interactable)
{
	FocusedInteractable = Interactable;
}

void APrototipoShooterCharacter::ClearFocusedInteractable(AShooterInteractable* Interactable)
{
	// so limpa se for o mesmo objeto (evita apagar outro que o jogador acabou de alcancar)
	if (FocusedInteractable.Get() == Interactable)
	{
		FocusedInteractable = nullptr;
	}
}

AShooterInteractable* APrototipoShooterCharacter::GetFocusedInteractable() const
{
	return FocusedInteractable.Get();
}

void APrototipoShooterCharacter::ShowScreenMessage(const FString& Message, float Duration)
{
	ScreenMessage = Message;
	ScreenMessageEndTime = GetWorld()->GetTimeSeconds() + Duration;
}

bool APrototipoShooterCharacter::GetActiveScreenMessage(FString& OutMessage) const
{
	if (ScreenMessage.IsEmpty() || GetWorld()->GetTimeSeconds() > ScreenMessageEndTime)
	{
		return false;
	}

	OutMessage = ScreenMessage;
	return true;
}
