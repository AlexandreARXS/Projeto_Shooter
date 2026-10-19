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

	virtual void BeginPlay() override;

	/** Chamado pelo personagem ao apertar o botao de interacao */
	void Interact(APrototipoShooterCharacter* Interactor);

protected:

	UFUNCTION()
	void OnAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:

	/** Destaca a caixa quando o jogador esta perto */
	void SetHighlighted(bool bHighlighted);
};
