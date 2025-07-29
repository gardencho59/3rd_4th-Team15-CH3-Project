#include "AI/System/Task/Focus/XVTASK_StartFocus.h"

// 추가됨
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "AI/DebugTool/DebugTool.h"
#include "GameFramework/Character.h"

UXVTASK_StartFocus::UXVTASK_StartFocus()
{
	// TODO : 플레이어 캐릭터 클래스로 변경할 것.
	NodeName = TEXT("Start Focus"); 
	LocationKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UXVTASK_StartFocus, LocationKey),AActor::StaticClass());
}

EBTNodeResult::Type UXVTASK_StartFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	if (LocationKey.SelectedKeyName.IsNone())
	{
		UE_LOG(Log_XV_AI, Error, TEXT("LocationKey is not set in Behavior Tree!"));
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	
	// TODO : 플레이어 캐릭터 클래스로 변경할 것.
	if (ACharacter* TargetActor = Cast<ACharacter>(BlackboardComp->GetValueAsObject(LocationKey.SelectedKeyName)))
	{
		AIController->SetFocus(TargetActor);
		return EBTNodeResult::Succeeded;
	}
	
	BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), false);
	return EBTNodeResult::Failed;
}
