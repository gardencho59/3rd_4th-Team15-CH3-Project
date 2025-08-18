#include "Weapon/GunBase.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/ProjectileBullet.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/XVCharacter.h"
#include "Inventory/Component/InventoryComponent.h"


AGunBase::AGunBase()
{
    PrimaryActorTick.bCanEverTick = true;

    GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
    RootComponent = GunMesh;

    bIsReloading = false;
    bCanFire = true;
    bIsEquiped = false;
    bSilencerAttached = false;
    bIsExtendedMagAttached = false;
    CurrentAmmo = 0;
    RemainingAmmo = 0;

    BaseSpread = 0.5f;          // 기본 퍼짐 (도 단위)
    MaxSpread = 7.0f;           // 최대 퍼짐
    CurrentSpread = BaseSpread; 
    SpreadIncrement = 0.2f;     // 발사시 증가량
    SpreadRecoveryRate = 1.5f;  // 초당 감소량
}

FVector AGunBase::GetAimDirection() const
{
    APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
    if (!PC) return FVector::ZeroVector;

    FVector WorldLocation;
    FVector WorldDirection;

    int32 ViewportX, ViewportY;
    PC->GetViewportSize(ViewportX, ViewportY);
    FVector2D ScreenCenter(ViewportX / 2.f, ViewportY / 2.f);

    if (PC->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
    {
        FHitResult Hit;
        FVector TraceEnd = WorldLocation + WorldDirection * 10000.0f;

        if (GetWorld()->LineTraceSingleByChannel(Hit, WorldLocation, TraceEnd, ECC_Visibility))
        {
            // 최소 거리 기준
            const float MinAimDistance = 300.0f;
            float DistanceToHit = FVector::Dist(WorldLocation, Hit.ImpactPoint);

            if (DistanceToHit >= MinAimDistance)
            {
                // 충분히 멀리 있으면, 그 지점으로 조준
                return (Hit.ImpactPoint - GetMuzzleLocation()).GetSafeNormal();
            }
            else
            {
                // 너무 가까우면 그냥 직선으로 발사
                return WorldDirection.GetSafeNormal();
            }
        }
        else
        {
            // 맞은 게 없으면 그냥 정면으로
            return WorldDirection.GetSafeNormal();
        }
    }

    return FVector::ZeroVector;
}

void AGunBase::SetAMMO(int32 SetAmmo)
{
    RemainingAmmo = SetAmmo;

    if (bIsEquiped)
    {
        OnMagAmmoChanged.Broadcast(CurrentAmmo, GetMagSize());
        OnReserveAmmoChanged.Broadcast(RemainingAmmo);
    }
}

FVector AGunBase::GetMuzzleLocation() const
{
    return GunMesh->GetSocketLocation(WeaponDataAsset->MuzzleSocketName);
}

void AGunBase::BeginPlay()
{
    Super::BeginPlay();

    if (WeaponDataAsset)
    {
        CurrentAmmo = WeaponDataAsset->MaxAmmo;
        DefaultMaxAmmo = CurrentAmmo;
        CurrentMaxAmmo = CurrentAmmo;

        if (WeaponDataAsset->WeaponMesh)
        {
            GunMesh->SetSkeletalMesh(WeaponDataAsset->WeaponMesh);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WeaponDataAsset is NULL on %s"), *GetName());
    }

    // ★ 시작 상태도 UI에 알려주기
    OnMagAmmoChanged.Broadcast(CurrentAmmo, GetMagSize());
    OnReserveAmmoChanged.Broadcast(RemainingAmmo);
}

void AGunBase::EmptyFireBullet()
{
    if (CurrentAmmo <= 0 && bCanFire)
    {
        bCanFire = false;
        PlaySoundAtMuzzle(WeaponDataAsset->EmptySound);
        GetWorld()->GetTimerManager().SetTimer(FireCooldownHandle, [this]()
        {
            bCanFire = true;
        }, WeaponDataAsset->FireRate, false);
    }
}


void AGunBase::FireBullet()
{
    if (!WeaponDataAsset || bIsReloading || !bCanFire)
        return;

    --CurrentAmmo;
    bCanFire = false;

    // ★ 탄창 이벤트 방송
    OnMagAmmoChanged.Broadcast(CurrentAmmo, GetMagSize());

    PlayEffects();
    SpawnBullet();

    // 🔥 발사할 때마다 퍼짐 증가
    CurrentSpread = FMath::Clamp(CurrentSpread + SpreadIncrement, BaseSpread, MaxSpread);

    // 퍼짐 회복 타이머 시작
    GetWorld()->GetTimerManager().SetTimer(SpreadRecoveryHandle, this, &AGunBase::RecoverSpread, 0.1f, true);

    GetWorld()->GetTimerManager().SetTimer(FireCooldownHandle, [this]()
    {
        bCanFire = true;
    }, WeaponDataAsset->FireRate, false);
}

void AGunBase::RecoverSpread()
{
    // 초당 SpreadRecoveryRate만큼 줄여줌
    CurrentSpread = FMath::Max(BaseSpread, CurrentSpread - (SpreadRecoveryRate * 0.1f));

    if (CurrentSpread <= BaseSpread + KINDA_SMALL_NUMBER)
    {
        GetWorld()->GetTimerManager().ClearTimer(SpreadRecoveryHandle);
    }
}

void AGunBase::Reload(int32 ReloadAmount)
{
    if (!WeaponDataAsset || bIsReloading)
        return;

    bIsReloading = true;
    bCanFire = false;
    PlaySoundAtMuzzle(WeaponDataAsset->ReloadSound);

    GetWorld()->GetTimerManager().SetTimer(
    ReloadTimerHandle,
    [this, ReloadAmount]() { FinishReload(ReloadAmount); },  // 람다로 감싸기
    WeaponDataAsset->ReloadTime,
    false
);

}

void AGunBase::FinishReload(int32 ReloadAmount)
{
    CurrentAmmo  += ReloadAmount;
    bIsReloading = false;
    bCanFire     = true;

    // ★ 둘 다 방송
    OnMagAmmoChanged.Broadcast(CurrentAmmo, GetMagSize());
    OnReserveAmmoChanged.Broadcast(RemainingAmmo);
}

void AGunBase::SpawnBullet()
{
    if (!WeaponDataAsset || !WeaponDataAsset->BulletClass) return;

    FVector MuzzleLocation = GetMuzzleLocation();
    FVector AimDirection = GetAimDirection();

    // 🔥 랜덤 오프셋 추가
    float RandomYaw   = FMath::RandRange(-CurrentSpread, CurrentSpread);
    float RandomPitch = FMath::RandRange(-CurrentSpread, CurrentSpread);

    FRotator SpreadRotation = AimDirection.Rotation();
    SpreadRotation.Yaw   += RandomYaw;
    SpreadRotation.Pitch += RandomPitch;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();

    AProjectileBullet* Bullet = GetWorld()->SpawnActor<AProjectileBullet>(
        WeaponDataAsset->BulletClass,
        MuzzleLocation,
        SpreadRotation,
        SpawnParams
    );

    if (Bullet)
    {
        Bullet->InitBullet(WeaponDataAsset->BulletSpeed, WeaponDataAsset->BulletDamage);
    }
}

void AGunBase::PlayEffects()
{
    if (bSilencerAttached)
    {
        PlaySoundAtMuzzle(WeaponDataAsset->SilenceSound);

        return;
    }
    if (!bSilencerAttached)
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
}

void AGunBase::AttachSilencer()
{
    if (!WeaponDataAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("WeaponDataAsset is NULL on %s"), *GetName());
        return;
    }

    if (!WeaponDataAsset->SilenceParts)
    {
        UE_LOG(LogTemp, Warning, TEXT("No SilenceParts mesh set in WeaponDataAsset for %s"), *GetName());
        return;
    }

    if (CurrentSilencer)
    {
        CurrentSilencer->DestroyComponent();
        CurrentSilencer = nullptr;
    }

    CurrentSilencer = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
    if (CurrentSilencer)
    {
        CurrentSilencer->RegisterComponent();
        CurrentSilencer->SetStaticMesh(WeaponDataAsset->SilenceParts);

        // 충돌 비활성화
        CurrentSilencer->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        CurrentSilencer->SetGenerateOverlapEvents(false);

        // 소켓 부착
        CurrentSilencer->AttachToComponent(GunMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Silence"));

        UE_LOG(LogTemp, Log, TEXT("Silencer attached on %s"), *GetName());
    }

    bSilencerAttached = true;
}

void AGunBase::DetachSilencer()
{
    if (CurrentSilencer)
    {
        CurrentSilencer->DestroyComponent();
        CurrentSilencer = nullptr;
        UE_LOG(LogTemp, Log, TEXT("Silencer detached from %s"), *GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No silencer to detach on %s"), *GetName());
    }

    bSilencerAttached = false;
}

void AGunBase::AttachExtendedMag()
{
    if (!bIsExtendedMagAttached)
    {
        CurrentMaxAmmo = DefaultMaxAmmo + (DefaultMaxAmmo / 3);

        if (CurrentAmmo > CurrentMaxAmmo)
        {
            CurrentAmmo = CurrentMaxAmmo;
        }

        bIsExtendedMagAttached = true;

        UE_LOG(LogTemp, Log, TEXT("Extended Magazine Attached: MaxAmmo %d -> %d"), DefaultMaxAmmo, CurrentMaxAmmo);
    }

    bIsExtendedMagAttached = true;
}

void AGunBase::DetachExtendedMag()
{
    if (bIsExtendedMagAttached && WeaponDataAsset)
    {
        // 원래 장탄 수로 복구
        CurrentMaxAmmo = DefaultMaxAmmo;

        if (CurrentAmmo > DefaultMaxAmmo)
        {
            CurrentAmmo = DefaultMaxAmmo;
            // ★ 시작 상태도 UI에 알려주기
            OnMagAmmoChanged.Broadcast(CurrentAmmo, GetMagSize());
            OnReserveAmmoChanged.Broadcast(RemainingAmmo);
        }

        bIsExtendedMagAttached = false;

        UE_LOG(LogTemp, Log, TEXT("Extended Magazine Detached: MaxAmmo restored to %d"), DefaultMaxAmmo);
    }

    bIsExtendedMagAttached = false;
}

void AGunBase::PlaySoundAtMuzzle(USoundBase* Sound) const
{
    if (!Sound) return;

    FVector MuzzleLoc = GunMesh->GetSocketLocation(WeaponDataAsset->MuzzleSocketName);
    UGameplayStatics::PlaySoundAtLocation(this, Sound, MuzzleLoc);
}

bool AGunBase::IsReloading() const
{
    return bIsReloading;
}
bool AGunBase::IsCanFire() const { return bCanFire; }

bool AGunBase::IsSilence() const { return bSilencerAttached; }
bool AGunBase::IsMag() const { return bIsExtendedMagAttached; }
bool AGunBase::IsEquiped(bool equiped)
{
    bIsEquiped = equiped;
    
    return bIsEquiped;
}

int32 AGunBase::GetCurrentMaxAmmo() const { return CurrentMaxAmmo; }
int32 AGunBase::GetRemainingAmmo() const { return RemainingAmmo; }
int32 AGunBase::GetCurrentAmmo() const { return CurrentAmmo; }

UAnimMontage* AGunBase::GetEquipMontage() const { return WeaponDataAsset->PlayerEquipAnimMontage; }
UAnimMontage* AGunBase::GetFireMontage() const { return WeaponDataAsset->PlayerFireAnimMontage; }
UAnimMontage* AGunBase::GetReloadMontage() const { return WeaponDataAsset->PlayerReloadAnimMontage; }

TSubclassOf<class UCameraShakeBase> AGunBase::GetCameraShake() const { return WeaponDataAsset->CameraShake; }
