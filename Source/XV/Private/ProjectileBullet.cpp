#include "ProjectileBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

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

	// 타이머로 탄환 자동 파괴 (예: 3초)
	SetLifeSpan(3.0f);
}

void AProjectileBullet::InitBullet(float Speed, float DamageValue)
{
	Damage = DamageValue;
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
							  UPrimitiveComponent* OtherComp, FVector NormalImpulse,
							  const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
		// 충돌 확인 로그
		UE_LOG(LogTemp, Warning, TEXT("Bullet HIT: %s"), *OtherActor->GetName());

		// 데미지 처리
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, GetVelocity().GetSafeNormal(), Hit, GetInstigatorController(), this, nullptr);

		// 탄환 파괴
		Destroy();
	}
}

