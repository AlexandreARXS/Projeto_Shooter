// Primeiro inimigo: patrulha, ve o jogador e atira em rajadas; leva dano no corpo e na cabeca, morre e reaparece

#include "ShooterEnemy.h"
#include "PrototipoShooterCharacter.h"
#include "ShooterDamageTypes.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimInstance.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/DamageEvents.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "AIController.h"
#include "ShooterWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "DrawDebugHelpers.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	/** Hitbox: invisivel, so bloqueia os tiros (canal Visibility) */
	void SetupHitbox(UShapeComponent* Hitbox)
	{
		Hitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Hitbox->SetCollisionResponseToAllChannels(ECR_Ignore);
		Hitbox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		Hitbox->SetGenerateOverlapEvents(false);
		Hitbox->SetCanEverAffectNavigation(false);
	}
}

AShooterEnemy::AShooterEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// um AIController simples "dirige" o personagem (necessario para o movimento funcionar)
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();

	// vira na direcao em que anda
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 300.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> QuinnMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple"));
	static ConstructorHelpers::FClassFinder<UAnimInstance> UnarmedAnim(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/ABP_Unarmed"));
	static ConstructorHelpers::FObjectFinder<UAnimSequence> Death1(TEXT("/Game/Characters/Mannequins/Anims/Death/MM_Death_Front_01.MM_Death_Front_01"));
	static ConstructorHelpers::FObjectFinder<UAnimSequence> Death2(TEXT("/Game/Characters/Mannequins/Anims/Death/MM_Death_Front_02.MM_Death_Front_02"));
	static ConstructorHelpers::FObjectFinder<UAnimSequence> Death3(TEXT("/Game/Characters/Mannequins/Anims/Death/MM_Death_Front_03.MM_Death_Front_03"));

	// capsula do personagem: mesmo tamanho do jogador; deixa os tiros passarem para as hitboxes
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	// malha da Quinn com a animacao de parado do template
	USkeletalMeshComponent* EnemyMesh = GetMesh();
	if (QuinnMesh.Succeeded())
	{
		EnemyMesh->SetSkeletalMeshAsset(QuinnMesh.Object);
	}
	EnemyMesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -96.f), FRotator(0.f, -90.f, 0.f));
	EnemyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (UnarmedAnim.Succeeded())
	{
		AliveAnimClass = UnarmedAnim.Class;
		EnemyMesh->SetAnimInstanceClass(AliveAnimClass);
	}

	if (Death1.Succeeded()) { DeathAnimations.Add(Death1.Object); }
	if (Death2.Succeeded()) { DeathAnimations.Add(Death2.Object); }
	if (Death3.Succeeded()) { DeathAnimations.Add(Death3.Object); }

	// corpo: do chao (Z -96) ate o pescoco (Z ~ +48)
	BodyHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BodyHitbox"));
	BodyHitbox->SetupAttachment(GetCapsuleComponent());
	BodyHitbox->InitCapsuleSize(28.f, 72.f);
	BodyHitbox->SetRelativeLocation(FVector(0.f, 0.f, -24.f));
	SetupHitbox(BodyHitbox);

	// PROVISORIO: Inimigo, hitboxes aproximadas (capsula do corpo + esfera da cabeca). Ajustar tamanhos ou usar o Physics Asset.
	// cabeca: presa ao osso "head"
	HeadHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("HeadHitbox"));
	HeadHitbox->SetupAttachment(EnemyMesh, TEXT("head"));
	HeadHitbox->InitSphereRadius(16.f);
	SetupHitbox(HeadHitbox);
}

void AShooterEnemy::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;

	// patrulha: de onde nasceu ate PatrolDistance para a direita, ida e volta
	PatrolStart = GetActorLocation();
	PatrolEnd = PatrolStart + GetActorRightVector() * PatrolDistance;
	SpawnRotation = GetActorRotation();
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;

	// espera um pouco antes de comecar a andar
	WaitUntil = GetWorld()->GetTimeSeconds() + PatrolWaitTime;

	// arma do inimigo
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Weapon = GetWorld()->SpawnActor<AShooterWeapon>(AShooterWeapon::StaticClass(), GetActorTransform(), SpawnParams);
	UpdateWeaponTransform();
}

void AShooterEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsDead)
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();

	// procura o jogador; em combate atira, senao patrulha
	UpdatePerception(Now);

	if (bEngaged)
	{
		UpdateCombat(Now, DeltaSeconds);
	}
	else
	{
		UpdatePatrol();
	}

	// levanta/abaixa a arma
	CombatAlpha = FMath::FInterpTo(CombatAlpha, bEngaged ? 1.f : 0.f, DeltaSeconds, 10.f);
	UpdateWeaponTransform();
}

FVector AShooterEnemy::GetEyeLocation() const
{
	return GetMesh()->GetSocketLocation(TEXT("head"));
}

void AShooterEnemy::UpdatePerception(float Now)
{
	APrototipoShooterCharacter* Player = Cast<APrototipoShooterCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	// sem jogador (ou jogador morto): sai do combate
	if (!Player || Player->IsDead())
	{
		bCanSeeTarget = false;
		if (bEngaged)
		{
			Disengage(Now);
		}
		return;
	}

	// em combate o inimigo acompanha o jogador mesmo fora do cone (so precisa da linha de visao)
	bCanSeeTarget = CanSeePlayer(Player, bEngaged);

	if (bCanSeeTarget)
	{
		LastSeenTime = Now;
		if (!bEngaged)
		{
			Engage(Player, Now);
		}
	}
	else if (bEngaged && Now - LastSeenTime > LoseSightTime)
	{
		Disengage(Now);
	}
}

bool AShooterEnemy::CanSeePlayer(const APrototipoShooterCharacter* Player, bool bIgnoreAngle) const
{
	const FVector Eye = GetEyeLocation();
	const FVector Forward = GetActorForwardVector();
	const float MinDot = FMath::Cos(FMath::DegreesToRadians(SightHalfAngle));

	FCollisionQueryParams Params(SCENE_QUERY_STAT(EnemySight), true, this);
	if (Weapon)
	{
		Params.AddIgnoredActor(Weapon);
	}

	// tenta enxergar a cabeca e o peito (agachado atras da cobertura, nenhum dos dois fica visivel)
	const FVector Points[] = { Player->GetHeadLocation(), Player->GetAimTargetLocation() };
	for (const FVector& Point : Points)
	{
		const FVector ToPoint = Point - Eye;
		if (ToPoint.Size() > SightRange)
		{
			continue;
		}

		// dentro do cone de visao?
		if (!bIgnoreAngle)
		{
			FVector Flat = ToPoint;
			Flat.Z = 0.f;
			if (FVector::DotProduct(Forward, Flat.GetSafeNormal()) < MinDot)
			{
				continue;
			}
		}

		// nada no meio do caminho? (a linha so "acerta" o jogador se uma hitbox dele estiver exposta)
		FHitResult Hit;
		const FVector End = Point + ToPoint.GetSafeNormal() * 20.f;
		if (GetWorld()->LineTraceSingleByChannel(Hit, Eye, End, ECC_Visibility, Params) && Hit.GetActor() == Player)
		{
			return true;
		}
	}

	return false;
}

void AShooterEnemy::Engage(APrototipoShooterCharacter* Player, float Now)
{
	TargetPlayer = Player;
	bEngaged = true;
	LastSeenTime = Now;

	// para de andar e da um tempo de reacao antes da primeira rajada
	GetCharacterMovement()->StopMovementImmediately();
	ShotsLeftInBurst = BurstSize;
	NextShotTime = FMath::Max(NextShotTime, Now + ReactionTime);
}

void AShooterEnemy::Disengage(float Now)
{
	bEngaged = false;
	TargetPlayer = nullptr;

	// volta a patrulha depois de uma pausa
	WaitUntil = Now + PatrolWaitTime;
	ResetStuckCheck(Now);
}

void AShooterEnemy::UpdateCombat(float Now, float DeltaSeconds)
{
	APrototipoShooterCharacter* Player = TargetPlayer.Get();
	if (!Player)
	{
		Disengage(Now);
		return;
	}

	// vira para o jogador
	FVector ToPlayer = Player->GetActorLocation() - GetActorLocation();
	ToPlayer.Z = 0.f;
	const FRotator Desired(0.f, ToPlayer.Rotation().Yaw, 0.f);
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), Desired, DeltaSeconds, 8.f));

	// so atira vendo o jogador e virado para ele
	const float FacingDot = FVector::DotProduct(GetActorForwardVector(), ToPlayer.GetSafeNormal());
	const bool bFacing = FacingDot >= FMath::Cos(FMath::DegreesToRadians(FireFacingTolerance));
	if (!bCanSeeTarget || !bFacing || Now < NextShotTime)
	{
		return;
	}

	// rajada: 3 tiros rapidos, depois uma pausa de 1,5 s
	FireAtTarget();
	ShotsLeftInBurst--;

	if (ShotsLeftInBurst > 0)
	{
		NextShotTime = Now + BurstShotInterval;
	}
	else
	{
		ShotsLeftInBurst = BurstSize;
		NextShotTime = Now + BurstPause;
	}
}

void AShooterEnemy::FireAtTarget()
{
	APrototipoShooterCharacter* Player = TargetPlayer.Get();
	if (!Weapon || !Player)
	{
		return;
	}

	UWorld* World = GetWorld();

	// mira no peito, com imprecisao
	const FVector Muzzle = Weapon->GetMuzzleLocation();
	const FVector AimDirection = (Player->GetAimTargetLocation() - Muzzle).GetSafeNormal();
	const FVector ShotDirection = FMath::VRandCone(AimDirection, FMath::DegreesToRadians(AimSpreadDegrees));
	const FVector End = Muzzle + ShotDirection * FireRange;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(EnemyFire), true, this);
	Params.AddIgnoredActor(Weapon);

	FHitResult Hit;
	const bool bHit = World->LineTraceSingleByChannel(Hit, Muzzle, End, ECC_Visibility, Params);
	const FVector EndPoint = bHit ? Hit.ImpactPoint : End;

	// PROVISORIO: Inimigo, efeitos do tiro com DrawDebug (rastro vermelho, clarao e impacto). Trocar por Niagara + som.
	DrawDebugLine(World, Muzzle, EndPoint, FColor(255, 70, 50), false, 0.08f, 0, 1.5f);
	DrawDebugSphere(World, Muzzle, 5.f, 6, FColor::Red, false, 0.05f);

	if (bHit)
	{
		DrawDebugPoint(World, Hit.ImpactPoint, 8.f, FColor(255, 120, 0), false, 0.8f);

		if (AActor* HitActor = Hit.GetActor())
		{
			UGameplayStatics::ApplyPointDamage(HitActor, ShotDamage, ShotDirection, Hit, GetController(), this, UDamageType::StaticClass());
		}
	}
}

void AShooterEnemy::UpdateWeaponTransform()
{
	if (!Weapon)
	{
		return;
	}

	// PROVISORIO: Inimigo, arma posicionada por codigo (as maos nao seguram). Trocar por animacoes de rifle + socket na mao.
	const FTransform ActorTransform = GetActorTransform();

	// descanso: na cintura, cano para baixo
	const FVector RestLocation = ActorTransform.TransformPosition(WeaponRestOffset);
	const FQuat RestRotation = ActorTransform.TransformRotation(WeaponRestRotation.Quaternion());

	// combate: no ombro, apontando para o jogador (ou para a frente)
	const FVector AimLocation = ActorTransform.TransformPosition(WeaponAimOffset);
	const APrototipoShooterCharacter* Player = TargetPlayer.Get();
	const FVector AimTarget = Player ? Player->GetAimTargetLocation() : AimLocation + GetActorForwardVector() * 1000.f;
	const FQuat AimRotation = (AimTarget - AimLocation).Rotation().Quaternion();

	Weapon->SetActorLocationAndRotation(
		FMath::Lerp(RestLocation, AimLocation, CombatAlpha),
		FQuat::Slerp(RestRotation, AimRotation, CombatAlpha));
}

void AShooterEnemy::UpdatePatrol()
{
	// parado (na ponta da patrulha, depois de levar tiro ou diante de obstaculo)
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now < WaitUntil)
	{
		ResetStuckCheck(Now);
		return;
	}

	const FVector PatrolTarget = bMovingToEnd ? PatrolEnd : PatrolStart;
	FVector ToTarget = PatrolTarget - GetActorLocation();
	ToTarget.Z = 0.f;

	// chegou: inverte o sentido e espera
	if (ToTarget.Size() < 40.f)
	{
		TurnAround(Now, PatrolWaitTime);
		return;
	}

	const FVector Direction = ToTarget.GetSafeNormal();

	// obstaculo a frente ou travado: volta pelo caminho de onde veio
	if (IsPathBlocked(Direction) || IsStuck(Now))
	{
		TurnAround(Now, ObstacleWaitTime);
		return;
	}

	AddMovementInput(Direction, 1.f);
}

bool AShooterEnemy::IsPathBlocked(const FVector& Direction) const
{
	// varre uma capsula um pouco menor que o inimigo, levemente acima do chao (para nao detectar o piso)
	const FVector Start = GetActorLocation() + FVector(0.f, 0.f, 10.f);
	const FVector End = Start + Direction * ObstacleProbeDistance;
	const FCollisionShape Shape = FCollisionShape::MakeCapsule(30.f, 70.f);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(EnemyObstacleProbe), false, this);

	// canal Pawn = tudo o que bloquearia um personagem andando (paredes, caixas, alvos, o jogador)
	FHitResult Hit;
	return GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Pawn, Shape, Params);
}

bool AShooterEnemy::IsStuck(float Now)
{
	if (Now - StuckCheckTime < StuckCheckInterval)
	{
		return false;
	}

	const float Moved = FVector::Dist2D(GetActorLocation(), StuckCheckLocation);
	ResetStuckCheck(Now);
	return Moved < StuckMinDistance;
}

void AShooterEnemy::TurnAround(float Now, float WaitTime)
{
	bMovingToEnd = !bMovingToEnd;
	WaitUntil = Now + WaitTime;
	GetCharacterMovement()->StopMovementImmediately();
	ResetStuckCheck(Now);
}

void AShooterEnemy::ResetStuckCheck(float Now)
{
	StuckCheckTime = Now;
	StuckCheckLocation = GetActorLocation();
}

void AShooterEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(RespawnTimer);

	if (Weapon)
	{
		Weapon->Destroy();
		Weapon = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

FVector AShooterEnemy::GetHealthBarLocation() const
{
	return GetActorLocation() + FVector(0.f, 0.f, 115.f);
}

float AShooterEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// morto nao leva dano (e nao gera marcador de acerto)
	if (bIsDead)
	{
		return 0.f;
	}

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// golpe corpo-a-corpo? (nao tem bonus de cabeca)
	const bool bMelee = DamageEvent.DamageTypeClass && DamageEvent.DamageTypeClass->IsChildOf(UShooterMeleeDamageType::StaticClass());

	// acertou a cabeca?
	bool bHeadshot = false;
	if (!bMelee && DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent& PointEvent = static_cast<const FPointDamageEvent&>(DamageEvent);
		bHeadshot = PointEvent.HitInfo.GetComponent() == HeadHitbox.Get();
	}

	const float Damage = DamageAmount * (bHeadshot ? HeadshotMultiplier : 1.f);
	Health = FMath::Max(0.f, Health - Damage);
	const bool bKilled = Health <= KINDA_SMALL_NUMBER;

	// pontos e feedback para o jogador
	if (APrototipoShooterCharacter* Shooter = Cast<APrototipoShooterCharacter>(DamageCauser))
	{
		Shooter->AddScore(bHeadshot ? HeadshotPoints : BodyHitPoints);
		if (bKilled)
		{
			Shooter->AddScore(KillPoints);
		}
		Shooter->NotifyEnemyHit(bHeadshot, bKilled, bMelee);
	}

	// PROVISORIO: Inimigo, mensagem de dano com AddOnScreenDebugMessage (para conferir a contagem de tiros).
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, bHeadshot ? FColor::Yellow : FColor::White,
			FString::Printf(TEXT("Inimigo: %s -%.0f   (vida %.0f/%.0f)"), bMelee ? TEXT("GOLPE") : (bHeadshot ? TEXT("CABECA") : TEXT("corpo")), Damage, Health, MaxHealth));
	}

	if (bKilled)
	{
		Die();
	}
	else
	{
		// "sente" o tiro: para por um instante
		GetCharacterMovement()->StopMovementImmediately();
		WaitUntil = FMath::Max(WaitUntil, GetWorld()->GetTimeSeconds() + HitPauseTime);

		// levou tiro do jogador: entra em combate mesmo sem ter visto (vira para quem atirou)
		if (APrototipoShooterCharacter* Shooter = Cast<APrototipoShooterCharacter>(DamageCauser))
		{
			if (!bEngaged && !Shooter->IsDead())
			{
				Engage(Shooter, GetWorld()->GetTimeSeconds());
			}
		}
	}

	return Damage;
}

void AShooterEnemy::Die()
{
	bIsDead = true;

	// para de receber tiros e deixa o jogador passar por cima
	SetHitboxesEnabled(false);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCharacterMovement()->DisableMovement();

	// sai do combate e esconde a arma
	bEngaged = false;
	bCanSeeTarget = false;
	TargetPlayer = nullptr;
	CombatAlpha = 0.f;
	if (Weapon)
	{
		Weapon->SetActorHiddenInGame(true);
	}

	// animacao de morte (fica parada no ultimo quadro)
	if (DeathAnimations.Num() > 0)
	{
		UAnimSequence* DeathAnim = DeathAnimations[FMath::RandRange(0, DeathAnimations.Num() - 1)];
		GetMesh()->PlayAnimation(DeathAnim, false);
	}

	GetWorldTimerManager().SetTimer(RespawnTimer, this, &AShooterEnemy::Respawn, RespawnDelay, false);
}

void AShooterEnemy::Respawn()
{
	bIsDead = false;
	Health = MaxHealth;

	// volta para o comeco da patrulha
	SetActorLocationAndRotation(PatrolStart, SpawnRotation, false, nullptr, ETeleportType::TeleportPhysics);
	bMovingToEnd = true;
	WaitUntil = GetWorld()->GetTimeSeconds() + PatrolWaitTime;

	SetHitboxesEnabled(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	// volta para a animacao de parado
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(AliveAnimClass);

	// arma de volta e combate zerado
	if (Weapon)
	{
		Weapon->SetActorHiddenInGame(false);
	}
	ShotsLeftInBurst = BurstSize;
	NextShotTime = 0.f;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Inimigo voltou"));
	}
}

void AShooterEnemy::SetHitboxesEnabled(bool bEnabled)
{
	const ECollisionEnabled::Type Mode = bEnabled ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision;
	BodyHitbox->SetCollisionEnabled(Mode);
	HeadHitbox->SetCollisionEnabled(Mode);
}
