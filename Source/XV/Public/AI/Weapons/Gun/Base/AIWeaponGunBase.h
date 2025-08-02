#pragma once

#include "CoreMinimal.h"
#include "AI/Weapons/Base/AIWeaponBase.h"
#include "AIWeaponGunBase.generated.h"

class UAudioComponent;

UCLASS()
class XV_API AAIWeaponGunBase : public AAIWeaponBase
{
	GENERATED_BODY()

public:
	AAIWeaponGunBase();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "AI")
	TObjectPtr<UAudioComponent> FireSound;
};
