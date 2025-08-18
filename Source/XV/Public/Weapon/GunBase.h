#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponDataAsset.h"
#include "WeaponInterface.h"
#include "GunBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMagAmmoChanged, int32, Current, int32, Max);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReserveAmmoChanged, int32, Reserve);

UCLASS()
class XV_API AGunBase : public AActor, public IWeaponInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category="Weapon|Ammo")
	FOnMagAmmoChanged OnMagAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category="Weapon|Ammo")
	FOnReserveAmmoChanged OnReserveAmmoChanged;
	
	UFUNCTION(BlueprintPure) int32 GetMagSize() const
	{
		return (WeaponDataAsset ? WeaponDataAsset->MaxAmmo : 0);
	}

	UFUNCTION(BlueprintPure) float GetMagPercent() const
	{
		const int32 Max = GetMagSize();
		return (Max > 0) ? float(CurrentAmmo) / Max : 0.f;
	}

	UFUNCTION(BlueprintCallable, Category="Weapon|Parts")
	void AttachExtendedMag();

	UFUNCTION(BlueprintCallable, Category="Weapon|Parts")
	void DetachExtendedMag();
	
	UFUNCTION(BlueprintCallable, Category = "Weapon|Attachment")
	void AttachSilencer();
	
	UFUNCTION(BlueprintCallable, Category = "Weapon|Attachment")
	void DetachSilencer();
	
	AGunBase();

	void SetAMMO(int32 SetAmmo);

	virtual void BeginPlay() override;

	UFUNCTION(Blueprintable)
	virtual void FireBullet() override;
	virtual void SpawnShellEject() override;
	
	virtual void EmptyFireBullet() override;
	virtual void Reload(int32 ReloadAmount) override;
	
	virtual bool IsReloading() const override;
	virtual bool IsCanFire() const override;
	
	bool IsSilence() const;
	bool IsMag() const;
	bool IsEquiped(bool equiped);
	
	virtual FVector GetAimDirection() const override;
	virtual FVector GetMuzzleLocation() const override;


	UFUNCTION(BlueprintPure, Category="Weapon|Ammo")
	virtual int32 GetCurrentMaxAmmo() const;
	UFUNCTION(BlueprintPure, Category="Weapon|Ammo")
	virtual int32 GetRemainingAmmo() const override;
	UFUNCTION(BlueprintPure, Category="Weapon|Ammo")
	virtual int32 GetCurrentAmmo() const override;

	virtual UWeaponDataAsset* GetWeaponData() const override { return WeaponDataAsset; }

	// 애니메이션
	virtual UAnimMontage* GetEquipMontage() const override;
	virtual UAnimMontage* GetFireMontage() const override;
	virtual UAnimMontage* GetReloadMontage() const override;
	
	virtual TSubclassOf<UCameraShakeBase> GetCameraShake() const override;

protected:
	void SpawnBullet();
	void PlayEffects();
	void PlaySoundAtMuzzle(USoundBase* Sound) const;
	void FinishReload(int32 ReloadAmount);

	float BaseSpread;
	float MaxSpread;
	float CurrentSpread;
	float SpreadIncrement;
	float SpreadRecoveryRate;

	FTimerHandle SpreadRecoveryHandle;

	void RecoverSpread();
	
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USkeletalMeshComponent* GunMesh;

	UPROPERTY()
	UStaticMeshComponent* CurrentSilencer;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	UWeaponDataAsset* WeaponDataAsset;

	int32 DefaultMaxAmmo;
	int32 CurrentAmmo;
	int32 CurrentMaxAmmo;
	int32 RemainingAmmo;
	bool bIsReloading;
	bool bIsEquiped;
	bool bCanFire;

	// 파츠 장착 상태
	bool bSilencerAttached;
	bool bIsExtendedMagAttached;

	FTimerHandle FireCooldownHandle;
	FTimerHandle ReloadTimerHandle;
};
