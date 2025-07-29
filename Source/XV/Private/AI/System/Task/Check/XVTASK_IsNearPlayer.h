#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "XVTASK_IsNearPlayer.generated.h"

/**
 * 
 */
UCLASS()
class XV_API UXVTASK_IsNearPlayer : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UXVTASK_IsNearPlayer();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector MyLocationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PlayerLocationKey;
};
