// Cobertura baixa: o jogador agacha automaticamente ao chegar perto

#include "ShooterCover.h"
#include "PrototipoShooterCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

namespace ShooterCoverSize
{
	// bloco: 60 cm de profundidade, 2,5 m de largura, 90 cm de altura (meia altura do personagem)
	constexpr float Depth = 60.f;
	constexpr float Width = 250.f;
	constexpr float Height = 90.f;

	// quanto a area de cobertura passa do bloco, para cada lado
	constexpr float ZoneMargin = 90.f;
}

AShooterCover::AShooterCover()
{
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	CoverRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CoverRoot"));
	RootComponent = CoverRoot;

	// bloco apoiado no chao (o cubo basico tem 100 cm e o centro no meio)
	Block = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Block"));
	Block->SetupAttachment(CoverRoot);
	Block->SetStaticMesh(CubeMesh.Object);
	Block->SetRelativeLocation(FVector(0.f, 0.f, ShooterCoverSize::Height * 0.5f));
	Block->SetRelativeScale3D(FVector(ShooterCoverSize::Depth / 100.f, ShooterCoverSize::Width / 100.f, ShooterCoverSize::Height / 100.f));

	// area de cobertura: so detecta o jogador
	CoverZone = CreateDefaultSubobject<UBoxComponent>(TEXT("CoverZone"));
	CoverZone->SetupAttachment(CoverRoot);
	CoverZone->SetBoxExtent(FVector(
		ShooterCoverSize::Depth * 0.5f + ShooterCoverSize::ZoneMargin,
		ShooterCoverSize::Width * 0.5f + ShooterCoverSize::ZoneMargin * 0.5f,
		60.f));
	CoverZone->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
	CoverZone->SetCollisionProfileName(TEXT("Trigger"));
	CoverZone->SetGenerateOverlapEvents(true);
}

void AShooterCover::BeginPlay()
{
	Super::BeginPlay();

	// cinza concreto
	Block->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(0.35f, 0.35f, 0.33f));

	CoverZone->OnComponentBeginOverlap.AddDynamic(this, &AShooterCover::OnZoneBeginOverlap);
	CoverZone->OnComponentEndOverlap.AddDynamic(this, &AShooterCover::OnZoneEndOverlap);
}

void AShooterCover::OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APrototipoShooterCharacter* ShooterCharacter = Cast<APrototipoShooterCharacter>(OtherActor))
	{
		ShooterCharacter->EnterCover(this);
	}
}

void AShooterCover::OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (APrototipoShooterCharacter* ShooterCharacter = Cast<APrototipoShooterCharacter>(OtherActor))
	{
		ShooterCharacter->LeaveCover(this);
	}
}
