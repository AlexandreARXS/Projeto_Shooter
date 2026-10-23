// Colecionavel: segure o botao de interacao para coletar; some da cena e aparece um icone na HUD

#include "ShooterCollectible.h"
#include "PrototipoShooterCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

AShooterCollectible::AShooterCollectible()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

	// medalha: disco de 36 cm em pe (o cilindro deitado de lado)
	if (CylinderMesh.Succeeded())
	{
		Box->SetStaticMesh(CylinderMesh.Object);
	}
	Box->SetRelativeScale3D(FVector(0.36f, 0.36f, 0.05f));
	Box->SetRelativeRotation(FRotator(0.f, 0.f, 90.f));
	Box->SetRelativeLocation(FVector(0.f, 0.f, FloatHeight));

	// nao bloqueia o jogador nem os tiros
	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// area menor que a da caixa de interacao
	InteractionArea->SetSphereRadius(150.f);
	InteractionArea->SetRelativeLocation(FVector(0.f, 0.f, FloatHeight));

	// segura o botao por 1 segundo
	HoldDuration = 1.f;
	Prompt = TEXT("[Segure E] Coletar");

	// dourado; mais brilhante quando o jogador esta perto
	NormalColor = FVector(0.75f, 0.5f, 0.05f);
	HighlightColor = FVector(1.f, 0.8f, 0.2f);
}

void AShooterCollectible::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// gira e flutua para chamar atencao
	RunningTime += DeltaSeconds;
	const float Yaw = FMath::Fmod(RunningTime * SpinSpeed * 360.f, 360.f);
	const float Bob = FMath::Sin(RunningTime * BobSpeed * PI) * BobHeight;

	Box->SetRelativeLocation(FVector(0.f, 0.f, FloatHeight + Bob));
	Box->SetRelativeRotation(FRotator(0.f, Yaw, 90.f));
}

void AShooterCollectible::Interact(APrototipoShooterCharacter* Interactor)
{
	if (!Interactor)
	{
		return;
	}

	// registra a coleta no jogador e some da cena
	Interactor->AddCollectible(CollectibleName);
	Interactor->ClearFocusedInteractable(this);
	Destroy();
}
