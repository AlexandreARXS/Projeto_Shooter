// Primeiro inimigo: boneco parado que leva dano no corpo e na cabeca, morre e reaparece

#include "ShooterEnemy.h"
#include "PrototipoShooterCharacter.h"
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
	PrimaryActorTick.bCanEverTick = false;

	// por enquanto nao tem IA: fica parado
	AutoPossessAI = EAutoPossessAI::Disabled;

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
}

void AShooterEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(RespawnTimer);
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

	// acertou a cabeca?
	bool bHeadshot = false;
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
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
		Shooter->NotifyEnemyHit(bHeadshot, bKilled);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, bHeadshot ? FColor::Yellow : FColor::White,
			FString::Printf(TEXT("Inimigo: %s -%.0f   (vida %.0f/%.0f)"), bHeadshot ? TEXT("CABECA") : TEXT("corpo"), Damage, Health, MaxHealth));
	}

	if (bKilled)
	{
		Die();
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

	SetHitboxesEnabled(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	// volta para a animacao de parado
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(AliveAnimClass);

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
