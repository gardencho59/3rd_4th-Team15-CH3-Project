#include "XVTASK_IsPlayerClosed.h"
#include "AIController.h"
#include "AI/AIComponents/AIConfigComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"

EBTNodeResult::Type UXVTASK_IsPlayerClosed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AI 컨트롤러, 소유 폰 체크
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    APawn* MyPawn = AIController->GetPawn();
    if (!MyPawn) return EBTNodeResult::Failed;

    // 플레이어 캐릭터 명확하게 얻기
    UWorld* World = MyPawn->GetWorld();
    if (!World) return EBTNodeResult::Failed;
    
    // 거리 계산
    const FVector AI_Location = MyPawn->GetActorLocation();
    const FVector Player_Location = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
    const float Distance = FVector::Dist(AI_Location, Player_Location);

    // AI 공격 범위 체크 (예시 코드)
    UAIConfigComponent* ConfigComp = MyPawn->FindComponentByClass<UAIConfigComponent>();
    float Attackrange = ConfigComp->AttackRange;

    if (Distance > Attackrange)
    {
        // 플레이어가 너무 멀다
        return EBTNodeResult::Failed;
    }

    // 도망 방향
    const FVector EscapeDir = (AI_Location - Player_Location).GetSafeNormal();
    const FVector TargetLocation = AI_Location + EscapeDir * MoveDistance;

    // 내비게이션에서 이동 지점 보정
    FNavLocation NavResult;
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
    
    if (NavSys && NavSys->ProjectPointToNavigation(TargetLocation, NavResult))
    {
        AIController->MoveToLocation(NavResult.Location);

        // 이동 멈췄는지 체크
        EPathFollowingStatus::Type MoveStatus = AIController->GetMoveStatus();
        if (MoveStatus == EPathFollowingStatus::Idle)
        {
            return EBTNodeResult::Succeeded;
        }

    }

    return EBTNodeResult::Failed;
}