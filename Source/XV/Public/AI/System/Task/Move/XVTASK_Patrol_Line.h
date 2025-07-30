#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "XVTASK_Patrol_Line.generated.h"

/**
 * 
 */
UCLASS()
class XV_API UXVTASK_Patrol_Line : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UXVTASK_Patrol_Line();

protected:
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector StartLocationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector EndLocationKey;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TogglePatrolLocation
;
};
