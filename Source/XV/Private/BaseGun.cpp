#include "BaseGun.h"

ABaseGun::ABaseGun()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentWeaponType = EWeaponType::None;
}

EWeaponType ABaseGun::GetWeaponType()
{
	return CurrentWeaponType;
}

void ABaseGun::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseGun::OnGunOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ABaseGun::OnGunEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ABaseGun::FireBullet()
{
}

FName ABaseGun::GetGunType() const
{
	return NAME_None;
}

