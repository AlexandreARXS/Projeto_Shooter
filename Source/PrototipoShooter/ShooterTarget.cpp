// Alvo circular de treino: mostra feedback quando e atingido

#include "ShooterTarget.h"
#include "PrototipoShooterCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/DamageEvents.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"

namespace ShooterTargetConstants
{
	// altura do centro do alvo e raios de cada anel (em unidades = cm)
	constexpr float TargetCenterHeight = 150.f;
	constexpr float TargetOuterRadius = 60.f;
	constexpr float TargetMiddleRadius = 42.f;
	constexpr float TargetInnerRadius = 25.f;
	constexpr float TargetBullseyeRadius = 10.f;

	// a face fica "deitada" para o cilindro virar um disco de frente para o X do ator
	const FRotator TargetFaceRotation(-90.f, 0.f, 0.f);

	const FVector TargetWhite(0.9f, 0.9f, 0.9f);
	const FVector TargetRed(0.8f, 0.05f, 0.05f);
	const FVector TargetFlash(1.f, 0.85f, 0.f);
}

using namespace ShooterTargetConstants;

AShooterTarget::AShooterTarget()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

	TargetRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TargetRoot"));
	RootComponent = TargetRoot;

	// poste do chao ate o centro do alvo, um pouco atras da face
	Post = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Post"));
	Post->SetupAttachment(TargetRoot);
	Post->SetStaticMesh(CubeMesh.Object);
	Post->SetRelativeLocation(FVector(-6.f, 0.f, TargetCenterHeight * 0.5f));
	Post->SetRelativeScale3D(FVector(0.08f, 0.08f, TargetCenterHeight / 100.f));

	Face = CreateDefaultSubobject<USceneComponent>(TEXT("Face"));
	Face->SetupAttachment(TargetRoot);
	Face->SetRelativeLocation(FVector(0.f, 0.f, TargetCenterHeight));
	Face->SetRelativeRotation(TargetFaceRotation);

	// discos empilhados, cada um um pouco mais a frente que o anterior
	RingOuter  = CreateRing(TEXT("RingOuter"),  CylinderMesh.Object, TargetOuterRadius * 2.f,    0.f);
	RingMiddle = CreateRing(TEXT("RingMiddle"), CylinderMesh.Object, TargetMiddleRadius * 2.f,   1.f);
	RingInner  = CreateRing(TEXT("RingInner"),  CylinderMesh.Object, TargetInnerRadius * 2.f,    2.f);
	Bullseye   = CreateRing(TEXT("Bullseye"),   CylinderMesh.Object, TargetBullseyeRadius * 2.f, 3.f);
}

UStaticMeshComponent* AShooterTarget::CreateRing(const TCHAR* Name, UStaticMesh* Mesh, float Diameter, float Depth)
{
	UStaticMeshComponent* Ring = CreateDefaultSubobject<UStaticMeshComponent>(Name);
	Ring->SetupAttachment(Face);
	Ring->SetStaticMesh(Mesh);
	Ring->SetRelativeLocation(FVector(0.f, 0.f, Depth));
	Ring->SetRelativeScale3D(FVector(Diameter / 100.f, Diameter / 100.f, 0.04f));
	return Ring;
}

void AShooterTarget::BeginPlay()
{
	Super::BeginPlay();

	Post->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(0.15f, 0.12f, 0.1f));
	SetRingColors(false);
}

void AShooterTarget::SetRingColors(bool bFlash)
{
	RingOuter->SetVectorParameterValueOnMaterials(TEXT("Color"), bFlash ? TargetFlash : TargetWhite);
	RingMiddle->SetVectorParameterValueOnMaterials(TEXT("Color"), bFlash ? TargetFlash : TargetRed);
	RingInner->SetVectorParameterValueOnMaterials(TEXT("Color"), bFlash ? TargetFlash : TargetWhite);
	Bullseye->SetVectorParameterValueOnMaterials(TEXT("Color"), bFlash ? TargetFlash : TargetRed);
}

float AShooterTarget::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// onde o tiro pegou?
	FVector HitPoint = Face->GetComponentLocation();
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent& PointEvent = static_cast<const FPointDamageEvent&>(DamageEvent);
		HitPoint = PointEvent.HitInfo.ImpactPoint;
	}

	// distancia ate o centro decide a pontuacao
	const float Distance = FVector::Dist(HitPoint, Face->GetComponentLocation());

	int32 Points = 10;
	const TCHAR* Label = TEXT("Na borda");
	FColor MessageColor = FColor::White;

	if (Distance <= TargetBullseyeRadius + 3.f)
	{
		Points = 100;
		Label = TEXT("NA MOSCA!");
		MessageColor = FColor::Yellow;
	}
	else if (Distance <= TargetInnerRadius + 3.f)
	{
		Points = 50;
		Label = TEXT("Quase no centro");
		MessageColor = FColor::Orange;
	}
	else if (Distance <= TargetMiddleRadius + 3.f)
	{
		Points = 25;
		Label = TEXT("Bom tiro");
	}

	Hits++;
	Score += Points;

	// soma os pontos no placar do jogador que atirou
	if (APrototipoShooterCharacter* Shooter = Cast<APrototipoShooterCharacter>(DamageCauser))
	{
		Shooter->AddScore(Points);
	}

	// feedback: clarao amarelo + balanco
	FlashTimeLeft = 0.12f;
	Wobble = 1.f;
	WobbleTime = 0.f;
	SetRingColors(true);

	// PROVISORIO: Alvo, mensagem com AddOnScreenDebugMessage (so aparece em builds de desenvolvimento). Trocar por widget na HUD.
	// feedback: mensagem na tela
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, MessageColor,
			FString::Printf(TEXT("Alvo %s: %s +%d   (acertos: %d | pontos: %d)"), *TargetName, Label, Points, Hits, Score));
	}

	return DamageAmount;
}

void AShooterTarget::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// fim do clarao
	if (FlashTimeLeft > 0.f)
	{
		FlashTimeLeft -= DeltaSeconds;
		if (FlashTimeLeft <= 0.f)
		{
			SetRingColors(false);
		}
	}

	// balanco para tras e para frente, perdendo forca
	if (Wobble > 0.f)
	{
		WobbleTime += DeltaSeconds;
		Wobble = FMath::Max(0.f, Wobble - DeltaSeconds * 2.5f);

		const float Angle = FMath::Sin(WobbleTime * 35.f) * 12.f * Wobble;
		Face->SetRelativeRotation(TargetFaceRotation + FRotator(Angle, 0.f, 0.f));
	}
}
