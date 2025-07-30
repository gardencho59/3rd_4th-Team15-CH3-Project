#include "AI/System/Task/Ranged/XVTask_Investigating.h"

UXVTask_Investigating::UXVTask_Investigating()
{
	NodeName = TEXT("Investigating");
}

EBTNodeResult::Type UXVTask_Investigating::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	return EBTNodeResult::Succeeded;
}
