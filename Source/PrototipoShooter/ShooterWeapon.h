// Arma provisoria, montada com formas basicas da engine

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterWeapon.generated.h"

class UStaticMeshComponent;

UCLASS()
class AShooterWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USceneComponent> WeaponRoot;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> Body;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> Barrel;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> Grip;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> Stock;

	/** Ponta do cano, de onde sai o tiro */
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USceneComponent> Muzzle;

public:

	AShooterWeapon();

	virtual void BeginPlay() override;

	/** Posicao da ponta do cano no mundo */
	FVector GetMuzzleLocation() const;

private:

	UStaticMeshComponent* CreatePart(const TCHAR* Name, UStaticMesh* Mesh, const FVector& Location, const FRotator& Rotation, const FVector& Scale);
};
