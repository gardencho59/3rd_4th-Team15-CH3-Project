#include "AI/Weapons/Gun/Base/AIWeaponGunBase.h"
#include "Components/AudioComponent.h"

AAIWeaponGunBase::AAIWeaponGunBase()
{
	PrimaryActorTick.bCanEverTick = true;

	FireSound = CreateDefaultSubobject<UAudioComponent>(TEXT("FireSound"));
	checkf(FireSound, TEXT("FireSound is nullptr"));
	FireSound->SetupAttachment(StaticMeshComponent);
	FireSound->bAutoActivate = false;
}

