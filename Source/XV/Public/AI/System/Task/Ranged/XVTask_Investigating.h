#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "XVTask_Investigating.generated.h"

UCLASS()
class XV_API UXVTask_Investigating : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UXVTask_Investigating();
	
protected:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
};