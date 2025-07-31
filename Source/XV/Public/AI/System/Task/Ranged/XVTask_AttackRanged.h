#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "XVTask_AttackRanged.generated.h"

UCLASS()
class XV_API UXVTask_AttackRanged : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UXVTask_AttackRanged();

protected:
	virtual EBTNodeResult:: Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
