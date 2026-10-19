// Alvo circular de treino: mostra feedback quando e atingido

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterTarget.generated.h"

class UStaticMeshComponent;

UCLASS()
class AShooterTarget : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USceneComponent> TargetRoot;

	/** Poste de apoio */
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> Post;

	/** Face do alvo (gira no feedback de acerto) */
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USceneComponent> Face;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> RingOuter;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> RingMiddle;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> RingInner;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> Bullseye;

public:

	AShooterTarget();

	/** Nome mostrado nas mensagens (Norte, Sul, ...) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Alvo")
	FString TargetName = TEXT("Alvo");

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:

	UStaticMeshComponent* CreateRing(const TCHAR* Name, UStaticMesh* Mesh, float Diameter, float Depth);

	/** Pinta os aneis com as cores normais, ou todos de amarelo no clarao de acerto */
	void SetRingColors(bool bFlash);

	int32 Hits = 0;
	int32 Score = 0;
	float FlashTimeLeft = 0.f;
	float Wobble = 0.f;
	float WobbleTime = 0.f;
};
