#include "GunBase.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectileBullet.h"
#include "NiagaraFunctionLibrary.h"

AGunBase::AGunBase()
{
    PrimaryActorTick.bCanEverTick = false;

    GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
    RootComponent = GunMesh;

    bIsReloading = false;
    bCanFire = true;
    CurrentAmmo = 0;
    RemainingAmmo = 100;
}

void AGunBase::BeginPlay()
{
    Super::BeginPlay();

    if (WeaponDataAsset)
    {
        CurrentAmmo = WeaponDataAsset->MaxAmmo;

        if (WeaponDataAsset->WeaponMesh)
        {
            GunMesh->SetSkeletalMesh(WeaponDataAsset->WeaponMesh);
            UE_LOG(LogTemp, Error, TEXT("WeaponDataAsset is NULL on %s"), *GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WeaponDataAsset is NULL on %s"), *GetName());
    }

}

void AGunBase::FireBullet()
{
    if (!WeaponDataAsset || bIsReloading || !bCanFire || CurrentAmmo <= 0)
    {
        PlaySoundAtMuzzle(WeaponDataAsset->EmptySound);
        return;
    }

    CurrentAmmo--;
    bCanFire = false;

    PlayEffects();
    SpawnBullet();

    GetWorld()->GetTimerManager().SetTimer(FireCooldownHandle, [this]()
    {
        bCanFire = true;
    }, WeaponDataAsset->FireRate, false);

    if (CurrentAmmo <= 0)
    {
        Reload();
    }
}

void AGunBase::Reload()
{
    if (!WeaponDataAsset || bIsReloading || RemainingAmmo <= 0)
        return;

    bIsReloading = true;

    PlaySoundAtMuzzle(WeaponDataAsset->ReloadSound);

    GetWorld()->GetTimerManager().SetTimer(
        ReloadTimerHandle,
        this,
        &AGunBase::FinishReload,
        WeaponDataAsset->ReloadTime,
        false
    );
}

void AGunBase::FinishReload()
{
    int32 MaxAmmo = WeaponDataAsset->MaxAmmo;
    int32 NeededAmmo = MaxAmmo - CurrentAmmo;
    int32 ReloadAmount = FMath::Min(NeededAmmo, RemainingAmmo);

    CurrentAmmo += ReloadAmount;
    RemainingAmmo -= ReloadAmount;
    bIsReloading = false;
}

void AGunBase::SpawnBullet()
{
    FVector MuzzleLoc = GunMesh->GetSocketLocation(WeaponDataAsset->MuzzleSocketName);
    FRotator MuzzleRot = GunMesh->GetSocketRotation(WeaponDataAsset->MuzzleSocketName);

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = GetInstigator();

    AProjectileBullet* Bullet = GetWorld()->SpawnActor<AProjectileBullet>(
        WeaponDataAsset->BulletClass,
        MuzzleLoc,
        MuzzleRot,
        Params
    );

    if (Bullet)
    {
        Bullet->InitBullet(1000.0f, 25.0f);
    }
}

void AGunBase::PlayEffects()
{
    if (WeaponDataAsset->MuzzleFlash)
    {
        UNiagaraFunctionLibrary::SpawnSystemAttached(
            WeaponDataAsset->MuzzleFlash,
            GunMesh,
            WeaponDataAsset->MuzzleSocketName,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::SnapToTarget,
            true
        );
    }

    PlaySoundAtMuzzle(WeaponDataAsset->FireSound);
}

void AGunBase::PlaySoundAtMuzzle(USoundBase* Sound)
{
    if (!Sound) return;

    FVector MuzzleLoc = GunMesh->GetSocketLocation(WeaponDataAsset->MuzzleSocketName);
    UGameplayStatics::PlaySoundAtLocation(this, Sound, MuzzleLoc);
}

bool AGunBase::IsReloading() const { return bIsReloading; }
int32 AGunBase::GetCurrentAmmo() const { return CurrentAmmo; }

UAnimMontage* AGunBase::GetEquipMontage() const { return WeaponDataAsset->PlayerEquipAnimMontage; }
UAnimMontage* AGunBase::GetFireMontage() const { return WeaponDataAsset->PlayerFireAnimMontage; }
UAnimMontage* AGunBase::GetReloadMontage() const { return WeaponDataAsset->PlayerReloadAnimMontage; }
