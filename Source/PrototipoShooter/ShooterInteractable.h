// Ponto de interacao: mostra um aviso quando o jogador chega perto e uma mensagem ao interagir

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterInteractable.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class APrototipoShooterCharacter;

UCLASS()
class AShooterInteractable : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USceneComponent> InteractableRoot;

	/** A caixa visivel */
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> Box;

	/** Area em volta da caixa onde a interacao fica disponivel */
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USphereComponent> InteractionArea;

public:

	AShooterInteractable();

	/** Mensagem mostrada ao interagir */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interacao")
	FString Message = TEXT("...");

	/** Aviso mostrado quando o jogador esta perto */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interacao")
	FString Prompt = TEXT("[E] Interagir");

	/** Quanto tempo a mensagem fica na tela */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interacao")
	float MessageDuration = 3.f;

	/** Segundos segurando o botao para interagir (0 = basta apertar) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interacao")
	float HoldDuration = 0.f;

	/** Cor normal e cor de destaque (quando o jogador esta perto) */
	UPROPERTY(EditAnywhere, Category="Interacao")
	FVector NormalColor = FVector(0.02f, 0.25f, 0.35f);

	UPROPERTY(EditAnywhere, Category="Interacao")
	FVector HighlightColor = FVector(0.1f, 0.7f, 1.f);

	virtual void BeginPlay() override;

	/** Chamado pelo personagem ao apertar (ou terminar de segurar) o botao de interacao */
	virtual void Interact(APrototipoShooterCharacter* Interactor);

protected:

	UFUNCTION()
	void OnAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Destaca o objeto quando o jogador esta perto */
	void SetHighlighted(bool bHighlighted);
};
