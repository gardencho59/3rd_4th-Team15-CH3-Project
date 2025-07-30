#include "AI/System/Task/Ranged/XVTask_AttackRanged.h"
#include "BehaviorTree/BlackboardComponent.h"


UXVTask_AttackRanged::UXVTask_AttackRanged()
{
	NodeName = TEXT("Ranged Attack");
}
EBTNodeResult::Type UXVTask_AttackRanged::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return EBTNodeResult::Failed;

	AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TEXT("Target")));
	if (!Target) return EBTNodeResult::Failed;

	UE_LOG(LogTemp, Log, TEXT("Ranged Attack"));

	return EBTNodeResult::InProgress;
}
