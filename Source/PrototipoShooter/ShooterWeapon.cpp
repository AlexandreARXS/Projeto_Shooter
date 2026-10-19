// Arma provisoria, montada com formas basicas da engine

#include "ShooterWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

AShooterWeapon::AShooterWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

	WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
	RootComponent = WeaponRoot;

	// X = direcao do cano. As formas basicas tem 100 unidades, por isso a escala 0.45 = 45 cm
	Body   = CreatePart(TEXT("Body"),   CubeMesh.Object,     FVector(5.f, 0.f, 0.f),    FRotator::ZeroRotator,        FVector(0.45f, 0.07f, 0.12f));
	Barrel = CreatePart(TEXT("Barrel"), CylinderMesh.Object, FVector(40.f, 0.f, 2.f),   FRotator(-90.f, 0.f, 0.f),    FVector(0.035f, 0.035f, 0.30f));
	Grip   = CreatePart(TEXT("Grip"),   CubeMesh.Object,     FVector(-8.f, 0.f, -11.f), FRotator(-15.f, 0.f, 0.f),    FVector(0.06f, 0.06f, 0.14f));
	Stock  = CreatePart(TEXT("Stock"),  CubeMesh.Object,     FVector(-24.f, 0.f, -2.f), FRotator::ZeroRotator,        FVector(0.14f, 0.05f, 0.09f));

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(WeaponRoot);
	Muzzle->SetRelativeLocation(FVector(56.f, 0.f, 2.f));
}

UStaticMeshComponent* AShooterWeapon::CreatePart(const TCHAR* Name, UStaticMesh* Mesh, const FVector& Location, const FRotator& Rotation, const FVector& Scale)
{
	UStaticMeshComponent* Part = CreateDefaultSubobject<UStaticMeshComponent>(Name);
	Part->SetupAttachment(WeaponRoot);
	Part->SetStaticMesh(Mesh);
	Part->SetRelativeLocation(Location);
	Part->SetRelativeRotation(Rotation);
	Part->SetRelativeScale3D(Scale);

	// a arma nao colide com nada (nao bloqueia o personagem nem os tiros)
	Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Part->SetGenerateOverlapEvents(false);
	return Part;
}

void AShooterWeapon::BeginPlay()
{
	Super::BeginPlay();

	// cores: corpo grafite, cano quase preto
	const FVector Graphite(0.06f, 0.06f, 0.07f);
	const FVector Black(0.02f, 0.02f, 0.02f);
	Body->SetVectorParameterValueOnMaterials(TEXT("Color"), Graphite);
	Grip->SetVectorParameterValueOnMaterials(TEXT("Color"), Black);
	Stock->SetVectorParameterValueOnMaterials(TEXT("Color"), Graphite);
	Barrel->SetVectorParameterValueOnMaterials(TEXT("Color"), Black);
}

FVector AShooterWeapon::GetMuzzleLocation() const
{
	return Muzzle->GetComponentLocation();
}
