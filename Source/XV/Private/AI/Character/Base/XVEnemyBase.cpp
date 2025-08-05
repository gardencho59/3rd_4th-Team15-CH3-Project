#include "AI/Character/Base/XVEnemyBase.h"

#include "BrainComponent.h"
#include "AI/Weapons/Base/AIWeaponBase.h"
#include "AI/AIComponents/AIStatusComponent.h"
#include "AI/System/AIController/Base/XVControllerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/AIComponents/AIConfigComponent.h"
#include "System/XVGameMode.h"

AXVEnemyBase::AXVEnemyBase()
	: RotateSpeed(480.f)
	, Acceleration(2048.f)
	, Deceleration(2048.f)
	, BrakingFriction(2.0f)
	, ControllerDesiredRotation(true)
	, OrientRotationToMovement(true)
	, AttackModeSpeed(400.f)
	, DestroyTime(5.f)
{
	// 컨트롤러 세팅
	AIControllerClass = AXVControllerBase::StaticClass();
	checkf(AIControllerClass != nullptr, TEXT("AIControllerClass is NULL"));

	// AI Config 가져오기
	AIConfigComponent = CreateDefaultSubobject<UAIConfigComponent>(TEXT("AIConfigComponent"));
	checkf(AIConfigComponent != nullptr, TEXT("AIConfigComponent is NULL"));

	// 스테이터스 가져오기
	AIStatusComponent = CreateDefaultSubobject<UAIStatusComponent>(TEXT("AIStatusComponent"));
	checkf(AIStatusComponent != nullptr, TEXT("AIStatusComponent is NULL"));
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AXVEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	// 세팅 설정
	AIConfigComponent->ConfigSetting();
	
	// MovementComponent 가져오기
	TObjectPtr<UCharacterMovementComponent> MovementComponent = CastChecked<UCharacterMovementComponent>(GetMovementComponent());
	checkf(MovementComponent != nullptr, TEXT("GetMovementComponent() returned NULL"));
	
	// 회전 속도 조정
	MovementComponent->RotationRate = FRotator(0.0f, RotateSpeed, 0.0f); // 부드러운 회전
         
	// 가속/감속 설정
	MovementComponent->MaxAcceleration = Acceleration;			  // 가속도
	MovementComponent->BrakingDecelerationWalking = Deceleration;  // 감속도
	MovementComponent->BrakingFrictionFactor = BrakingFriction;    // 마찰 계수
         
	// 이동 보간 설정
	MovementComponent->bUseControllerDesiredRotation = ControllerDesiredRotation;  // 컨트롤러 방향으로 부드럽게 회전
	MovementComponent->bOrientRotationToMovement = OrientRotationToMovement;       // 이동 방향으로 캐릭터 회전

	// 무기 끼우기
	// TODO : 무기 어떻게 할거임? 기존의 것 가져오던지 아니면 말던지.
	SetWeapon();
	// checkf(AIWeaponBaseClass != nullptr, TEXT("AIWeaponBaseClass is NULL"));

}

void AXVEnemyBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// OnEnemyKilled 호출
	if(AXVGameMode* GameMode = GetWorld()->GetAuthGameMode<AXVGameMode>())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnEnemyKilled"));
		GameMode->OnEnemyKilled();
	}
		
	Super::EndPlay(EndPlayReason);
}

void AXVEnemyBase::Destroyed()
{
	// 무기 파괴
	if (AIWeaponBase)
	{
		AIWeaponBase->Destroy();
	}
	
	Super::Destroyed();
}

void AXVEnemyBase::SetWeapon()
{
	if (AIWeaponBaseClass)
	{
		// 무기 소환
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		AIWeaponBase = GetWorld()->SpawnActor<AAIWeaponBase>
		(
			AIWeaponBaseClass, 
			GetActorLocation(), 
			GetActorRotation(), 
			SpawnParams
		);

		// 무기를 소켓 등에 Attach하기
		if (AIWeaponBase)
		{
			AIWeaponBase->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("WeaponSocket")));
		}
	}
}

void AXVEnemyBase::GetDamage(float Damage)
{
	AIStatusComponent->Sub_Health(Damage);
	
	// ▼ 체력이 0 이하로 떨어졌을 때만 사망 처리!
	if (AIStatusComponent->CurrentHealth() <= 0.f)
	{
		// 컨트롤러 가져오기 (현재 액터에 바운드된 실제 컨트롤러)
		AXVControllerBase* AIController = Cast<AXVControllerBase>(GetController());
		if (AIController)
		{
			// 1. 움직임 멈춤
			AIController->StopMovement();

			// 2. 비헤이비어 트리(BrainComponent) 정지
			if (AIController->BrainComponent)
			{
				AIController->BrainComponent->StopLogic(TEXT("Dead"));
			}
		}

		// 죽는 모션이 있으면 우선 재생
		if (DeathMontage)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				// 델리게이트 람다 바인딩
				FOnMontageEnded MontageEndedDelegate;
				MontageEndedDelegate.BindLambda([this](UAnimMontage*, bool)
				{
					DeathTimer();
				});
				
				AnimInstance->Montage_Play(DeathMontage);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, DeathMontage);
			}
			else
			{
				DeathTimer();
			}
		}
		else
		{
			DeathTimer();
		}
	}
}

void AXVEnemyBase::SetAttackMode()
{
	// MovementComponent 가져오기
	UCharacterMovementComponent* MovementComponent = CastChecked<UCharacterMovementComponent>(GetMovementComponent());
	checkf(MovementComponent != nullptr, TEXT("GetMovementComponent() returned NULL"));

	// 공격모드 속도로 설정
	MovementComponent->MaxWalkSpeed = AttackModeSpeed;
	
}

void AXVEnemyBase::DeathTimer()
{
	FTimerHandle DeathTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		DeathTimerHandle,         // 타이머 핸들
		[this]()	 // 타이머 만료 시 실행할 람다(또는 함수)
		{             
			Destroy();				 // 디스트로이
		},
		DestroyTime,				 // 대기 시간(초)
		false						 // 반복 여부(false: 1회)
	);
}