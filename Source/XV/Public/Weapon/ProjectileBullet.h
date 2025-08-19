#pragma once

#include "CoreMinimal.h"
#include "GunBase.h"
#include "GameFramework/Actor.h"
#include "ProjectileBullet.generated.h"

class UProjectileMovementComponent;

UCLASS()
class XV_API AProjectileBullet : public AActor
{
	GENERATED_BODY()

public:    
	AProjectileBullet();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BulletMesh;

	UPROPERTY(EditAnywhere, Category = "VFX")
	UMaterialInterface* BulletImpactDecal;

	UPROPERTY(EditDefaultsOnly, Category="Effect")
	class UNiagaraSystem* BulletImpactNiagara;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;

	AGunBase* GunBase;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
			   UPrimitiveComponent* OtherComp, FVector NormalImpulse,
			   const FHitResult& Hit);

public:
	void InitBullet(AGunBase* Gun, float Speed, float DamageValue);

private:
	float Damage;
};
