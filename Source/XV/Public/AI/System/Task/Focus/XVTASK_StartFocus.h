#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "XVTASK_StartFocus.generated.h"

/**
 * 
 */
UCLASS()
class XV_API UXVTASK_StartFocus : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UXVTASK_StartFocus();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LocationKey;
};
