#include "Weapon/ProjectileBullet.h"

#include "AI/Character/Base/XVEnemyBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/GunBase.h"

AProjectileBullet::AProjectileBullet()
{
	PrimaryActorTick.bCanEverTick = false;

	// 1. Bullet Mesh 생성 및 콜리전 설정
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	RootComponent = BulletMesh;

	// 충돌 이벤트 활성화
	BulletMesh->SetNotifyRigidBodyCollision(true);
	BulletMesh->SetSimulatePhysics(false);
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BulletMesh->SetCollisionObjectType(ECC_GameTraceChannel1); // 예: Projectile용 채널
	BulletMesh->SetCollisionResponseToAllChannels(ECR_Block); // 모든 것과 Block
	BulletMesh->BodyInstance.bUseCCD = true; // 고속 충돌 보정 (관통 방지)

	// 움직일 수 있다 알려주는
	BulletMesh->SetMobility(EComponentMobility::Movable);

	// OnHit 이벤트 바인딩
	BulletMesh->OnComponentHit.AddDynamic(this, &AProjectileBullet::OnHit);

	// 2. Projectile Movement 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f; // 필요 시 1.0f로 중력 활성화
}


void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();

	// 처음엔 안 보이게
	BulletMesh->SetVisibility(false);

	// 0.2초 후 보이게
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		BulletMesh->SetVisibility(true);
	}, 0.05f, false);

	// 기존처럼 3초 후 자동 파괴
	SetLifeSpan(1.5f);
}

void AProjectileBullet::InitBullet(AGunBase* Gun, float Speed, float DamageValue)
{
	Damage = DamageValue;
	GunBase = Gun;
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
							  UPrimitiveComponent* OtherComp, FVector NormalImpulse,
							  const FHitResult& Hit)
{
	
	
	if (OtherActor && OtherActor != this)
	{
		if (AXVEnemyBase* Enemy = Cast<AXVEnemyBase>(OtherActor))
		{
			Enemy->GetDamage(Damage);

			GunBase->EnemyHit();
		}

		if (BulletImpactDecal)
		{
			// 디칼 붙이기
			UGameplayStatics::SpawnDecalAtLocation(
				GetWorld(),
				BulletImpactDecal,
				FVector(10.f, 10.f, 10.f),     // 디칼 크기
				Hit.ImpactPoint,               // 부딪힌 지점
				Hit.ImpactNormal.Rotation(),   // 방향
				10.0f                          // 지속 시간
			);
		}

		// OnHit 안에 추가
		if (BulletImpactNiagara)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				BulletImpactNiagara,       // 파티클 시스템
				Hit.ImpactPoint,            // 위치
				Hit.ImpactNormal.Rotation() // 방향
			);
		}

		 
		// 프렉처 관련 오류 남
		if (OtherComp && OtherComp->IsSimulatingPhysics())
		{
			FVector Impulse = GetVelocity() * 1000.0f;
			OtherComp->AddImpulseAtLocation(Impulse, Hit.ImpactPoint);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No physics applied to %s (Static or no SimulatePhysics)"), *OtherComp->GetName());
		}
		
		// 충돌 확인 로그
		UE_LOG(LogTemp, Warning, TEXT("Bullet HIT: %s"), *OtherActor->GetName());

		// 데미지 처리
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, GetVelocity().GetSafeNormal(), Hit, GetInstigatorController(), this, nullptr);

		// 탄환 파괴
		Destroy();
	}
}

