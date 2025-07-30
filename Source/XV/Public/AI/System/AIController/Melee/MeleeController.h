#pragma once

#include "CoreMinimal.h"
#include "AI/System/AIController/Base/XVControllerBase.h"
#include "MeleeController.generated.h"

UCLASS()
class XV_API AMeleeController : public AXVControllerBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

protected:
	
	UFUNCTION()
	void OnTargetUpdated(AActor* Actor, FAIStimulus Stimulus); // 감지한 타겟 정보 업데이트
	
};
