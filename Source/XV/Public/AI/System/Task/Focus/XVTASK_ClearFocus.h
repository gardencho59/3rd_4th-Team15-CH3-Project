#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "XVTASK_ClearFocus.generated.h"

/**
 * 
 */
UCLASS()
class XV_API UXVTASK_ClearFocus : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UXVTASK_ClearFocus();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
