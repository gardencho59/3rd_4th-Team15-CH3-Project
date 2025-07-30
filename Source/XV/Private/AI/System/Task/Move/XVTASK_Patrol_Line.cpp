// 패트롤 있기 떄문에 개발 중단
// 패트롤 있기 떄문에 개발 중단
// 패트롤 있기 떄문에 개발 중단
// 패트롤 있기 떄문에 개발 중단
// 패트롤 있기 떄문에 개발 중단
// 패트롤 있기 떄문에 개발 중단
// 패트롤 있기 떄문에 개발 중단
// 패트롤 있기 떄문에 개발 중단
// 패트롤 있기 떄문에 개발 중단
// 패트롤 있기 떄문에 개발 중단
// 패트롤 있기 떄문에 개발 중단
// 패트롤 있기 떄문에 개발 중단
// 패트롤 있기 떄문에 개발 중단

#include "AI/System/Task/Move/XVTASK_Patrol_Line.h"

// 추가됨
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "AI/DebugTool/DebugTool.h"
#include "Navigation/PathFollowingComponent.h"

UXVTASK_Patrol_Line::UXVTASK_Patrol_Line()
{
	NodeName = TEXT("Find Random Location");
	StartLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UXVTASK_Patrol_Line, StartLocationKey));
	EndLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UXVTASK_Patrol_Line, EndLocationKey));
	EndLocationKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UXVTASK_Patrol_Line, TogglePatrolLocation));
}

EBTNodeResult::Type UXVTASK_Patrol_Line::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 오너 확인
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	// 폰 존재 확인
	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn) return EBTNodeResult::Failed;

	// 네브 메쉬 확인
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem) return EBTNodeResult::Failed;

	// 키 확인
	if (StartLocationKey.SelectedKeyName.IsNone() && EndLocationKey.SelectedKeyName.IsNone())
	{
		UE_LOG(Log_XV_AI, Error, TEXT("LocationKeys are not set in Behavior Tree!"));
		return EBTNodeResult::Failed;
	}

	// 블랙보드 컴포넌트 가져오기
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	// true == 스타트 지점에서만 시작
	// 다 도착하면 false 로 변경 
	if(true == BlackboardComp->GetValueAsBool(TEXT("AIIsAttacking")))
	{
		
	}

	// flase == 엔드 지점에서 시작
	// 다 도착하면 true 로 변경 
	if(false == BlackboardComp->GetValueAsBool(TEXT("AIIsAttacking")))
	{
		
	}
	
	// 시작 위치 가져오기
	FVector StartLocation = BlackboardComp->GetValueAsVector(StartLocationKey.SelectedKeyName);
    
	// 끝 위치 가져오기
	FVector EndLocation = BlackboardComp->GetValueAsVector(EndLocationKey.SelectedKeyName);

	// 이동 요청 생성
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(StartLocation);				// 목표 지점
	MoveRequest.SetAcceptanceRadius(10.f);					// 얼마나 가까이 가야 도착으로 간주할지
	MoveRequest.SetReachTestIncludesAgentRadius(true);		// 콜리전 반경 고려 여부 설정
	MoveRequest.SetUsePathfinding(true);					// 경로 탐색 사용
	MoveRequest.SetAllowPartialPath(true);					// 부분 경로 허용
	
	// 이동 실행
	AIController->MoveTo(MoveRequest);
	
	UE_LOG(Log_XV_AI, Warning, TEXT("Find Random Location Failed")); 
	return EBTNodeResult::Failed;  // 실패
}