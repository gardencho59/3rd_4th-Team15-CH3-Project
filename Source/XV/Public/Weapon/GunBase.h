#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponDataAsset.h"
#include "WeaponInterface.h"
#include "GunBase.generated.h"

UCLASS()
class XV_API AGunBase : public AActor, public IWeaponInterface
{
	GENERATED_BODY()

public:
	AGunBase();

	virtual void BeginPlay() override;

	virtual void FireBullet() override;
	virtual void Reload() override;
	
	virtual bool IsReloading() const override;
	virtual bool IsCanFire() const override;
	
	virtual FVector GetAimDirection() const override;
	virtual FVector GetMuzzleLocation() const override;
	
	virtual int32 GetRemainingAmmo() const override;
	virtual int32 GetCurrentAmmo() const override;

	virtual UWeaponDataAsset* GetWeaponData() const override { return WeaponDataAsset; }

	// 애니메이션
	virtual UAnimMontage* GetEquipMontage() const override;
	virtual UAnimMontage* GetFireMontage() const override;
	virtual UAnimMontage* GetReloadMontage() const override;

protected:
	void SpawnBullet();
	void PlayEffects();
	void PlaySoundAtMuzzle(USoundBase* Sound) const;
	void FinishReload();
	
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USkeletalMeshComponent* GunMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	UWeaponDataAsset* WeaponDataAsset;

	int32 CurrentAmmo;
	int32 RemainingAmmo;
	bool bIsReloading;
	bool bCanFire;

	FTimerHandle FireCooldownHandle;
	FTimerHandle ReloadTimerHandle;
};
