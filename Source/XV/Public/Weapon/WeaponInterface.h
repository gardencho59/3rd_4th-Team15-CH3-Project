#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

class XV_API IWeaponInterface
{
	GENERATED_BODY()

public:
	virtual void EmptyFireBullet() = 0;
	virtual void FireBullet() = 0;
	virtual void SpawnShellEject() = 0;
	virtual void Reload(int32 ReloadAmount) = 0;
	virtual bool IsReloading() const = 0;
	virtual bool IsCanFire() const = 0;
	virtual FVector GetAimDirection() const = 0;
	virtual FVector GetMuzzleLocation() const = 0;
	virtual int32 GetRemainingAmmo() const = 0;
	virtual int32 GetCurrentAmmo() const = 0;

	virtual class UWeaponDataAsset* GetWeaponData() const = 0;

	// 애니메이션
	virtual class UAnimMontage* GetEquipMontage() const = 0;
	virtual class UAnimMontage* GetFireMontage() const = 0;
	virtual class UAnimMontage* GetReloadMontage() const = 0;

	virtual TSubclassOf<class UCameraShakeBase> GetCameraShake() const = 0;
};
