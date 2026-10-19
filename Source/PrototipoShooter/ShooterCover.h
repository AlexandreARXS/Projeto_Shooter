// Cobertura baixa: o jogador agacha automaticamente ao chegar perto

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterCover.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

// PROVISORIO: Cobertura, bloco cinza de forma basica. Trocar por modelos do cenario (muros, carros, sacos de areia...).
UCLASS()
class AShooterCover : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USceneComponent> CoverRoot;

	/** O bloco de cobertura (solido: bloqueia o jogador e os tiros) */
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> Block;

	/** Area em volta do bloco onde o jogador entra em cobertura */
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UBoxComponent> CoverZone;

public:

	AShooterCover();

	virtual void BeginPlay() override;

protected:

	UFUNCTION()
	void OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
