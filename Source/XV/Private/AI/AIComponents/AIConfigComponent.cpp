#include "AI/AIComponents/AIConfigComponent.h"

#include "AI/System/AIController/Base/XVControllerBase.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"

void UAIConfigComponent::ConfigSetting()
{
	// 소유한 폰(캐릭터) 가져오기
	APawn* Owner = Cast<APawn>(GetOwner());
	if (!Owner) return;

	// AI 컨트롤러 가져오기
	AXVControllerBase* AIController = Cast<AXVControllerBase>(Owner->GetController());
	if (!AIController) return;

    // SightConfig에 DataAsset 값 적용
	AIController->AISightConfig->SightRadius = AISightRadius;
	AIController->AISightConfig->LoseSightRadius = AILoseSightRadius;
	AIController->AISightConfig->PeripheralVisionAngleDegrees = AIPeripheralVisionAngleDegrees;
	AIController->AISightConfig->SetMaxAge(AISightSetMaxAge);

	// HearingConfig에 DataAsset 값 적용
	AIController->AIHearingConfig->HearingRange = AIHearingRange;
	AIController->AIHearingConfig->SetMaxAge(AIHearingSetMaxAge);

	// 감지 설정 적용 (눈)
	AIController->AISightConfig->DetectionByAffiliation.bDetectEnemies = AIbDetectEnemies;
	AIController->AISightConfig->DetectionByAffiliation.bDetectNeutrals = AIbDetectNeutrals;
	AIController->AISightConfig->DetectionByAffiliation.bDetectFriendlies = AIbDetectFriendlies;

	// 감지 설정 적용 (귀)
	AIController->AISightConfig->DetectionByAffiliation.bDetectEnemies = AIbDetectEnemies;
	AIController->AISightConfig->DetectionByAffiliation.bDetectNeutrals = AIbDetectNeutrals;
	AIController->AISightConfig->DetectionByAffiliation.bDetectFriendlies = AIbDetectFriendlies;

	AIController->AIPerception->RequestStimuliListenerUpdate();
}



