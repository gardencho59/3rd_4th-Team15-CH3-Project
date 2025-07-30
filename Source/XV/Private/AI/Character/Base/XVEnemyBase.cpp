#include "AI/Character/Base/XVEnemyBase.h"

// 추가됨
#include "AI/System/AIController/Base/XVControllerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

AXVEnemyBase::AXVEnemyBase()
	: RoateSpeed(480.f)
	, Acceleration(2048.f)
	, Deceleration(2048.f)
	, BrakingFriction(2.0f)
	, ControllerDesiredRotation(true)
	, OrientRotationToMovement(true)
{
	AIControllerClass = AXVControllerBase::StaticClass();
	check(AIControllerClass);
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AXVEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	// MovementComponent 가져오기
	MovementComp = Cast<UCharacterMovementComponent>(GetMovementComponent());

	// 회전 속도 조정
	MovementComp->RotationRate = FRotator(0.0f, RoateSpeed, 0.0f); // 부드러운 회전
          
	// 가속/감속 설정
	MovementComp->MaxAcceleration = Acceleration;			  // 가속도
	MovementComp->BrakingDecelerationWalking = Deceleration;  // 감속도
	MovementComp->BrakingFrictionFactor = BrakingFriction;    // 마찰 계수
          
	// 이동 보간 설정
	MovementComp->bUseControllerDesiredRotation = ControllerDesiredRotation;  // 컨트롤러 방향으로 부드럽게 회전
	MovementComp->bOrientRotationToMovement = OrientRotationToMovement;       // 이동 방향으로 캐릭터 회전
	
}
