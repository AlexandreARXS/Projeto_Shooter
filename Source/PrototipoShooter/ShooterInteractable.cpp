// Ponto de interacao: mostra um aviso quando o jogador chega perto e uma mensagem ao interagir

#include "ShooterInteractable.h"
#include "PrototipoShooterCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

AShooterInteractable::AShooterInteractable()
{
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	InteractableRoot = CreateDefaultSubobject<USceneComponent>(TEXT("InteractableRoot"));
	RootComponent = InteractableRoot;

	// caixa de 80 cm apoiada no chao
	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Box"));
	Box->SetupAttachment(InteractableRoot);
	Box->SetStaticMesh(CubeMesh.Object);
	Box->SetRelativeLocation(FVector(0.f, 0.f, 40.f));
	Box->SetRelativeScale3D(FVector(0.8f, 0.8f, 0.8f));

	// area de interacao: so detecta, nao bloqueia
	InteractionArea = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionArea"));
	InteractionArea->SetupAttachment(InteractableRoot);
	InteractionArea->SetRelativeLocation(FVector(0.f, 0.f, 40.f));
	InteractionArea->InitSphereRadius(220.f);
	InteractionArea->SetCollisionProfileName(TEXT("Trigger"));
	InteractionArea->SetGenerateOverlapEvents(true);
}

void AShooterInteractable::BeginPlay()
{
	Super::BeginPlay();

	InteractionArea->OnComponentBeginOverlap.AddDynamic(this, &AShooterInteractable::OnAreaBeginOverlap);
	InteractionArea->OnComponentEndOverlap.AddDynamic(this, &AShooterInteractable::OnAreaEndOverlap);

	SetHighlighted(false);
}

void AShooterInteractable::SetHighlighted(bool bHighlighted)
{
	Box->SetVectorParameterValueOnMaterials(TEXT("Color"), bHighlighted ? HighlightColor : NormalColor);
}

void AShooterInteractable::OnAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APrototipoShooterCharacter* ShooterCharacter = Cast<APrototipoShooterCharacter>(OtherActor))
	{
		ShooterCharacter->SetFocusedInteractable(this);
		SetHighlighted(true);
	}
}

void AShooterInteractable::OnAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (APrototipoShooterCharacter* ShooterCharacter = Cast<APrototipoShooterCharacter>(OtherActor))
	{
		ShooterCharacter->ClearFocusedInteractable(this);
		SetHighlighted(false);
	}
}

void AShooterInteractable::Interact(APrototipoShooterCharacter* Interactor)
{
	if (Interactor)
	{
		Interactor->ShowScreenMessage(Message, MessageDuration);
	}
}
