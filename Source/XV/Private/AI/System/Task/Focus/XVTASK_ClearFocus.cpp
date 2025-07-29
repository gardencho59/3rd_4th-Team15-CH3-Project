#include "AI/System/Task/Focus/XVTASK_ClearFocus.h"

// 추가됨
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UXVTASK_ClearFocus::UXVTASK_ClearFocus()
{
	NodeName = TEXT("Clear Focus"); 
}

EBTNodeResult::Type UXVTASK_ClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 오너 확인
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;
	
	// 폰 존재 확인
	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn) return EBTNodeResult::Failed;

	AIController->ClearFocus(EAIFocusPriority::Gameplay);
	AIController->SetFocalPoint(FVector::ZeroVector);

	return EBTNodeResult::Succeeded;

}
