#include "AI/Character/Base/XVEnemyBase.h"

// 추가됨
#include "AI/System/AIController/Base/XVControllerBase.h"
#include  "AI/AIComponents/AIConfigComponent.h"

AXVEnemyBase::AXVEnemyBase()
{
	AIControllerClass = AXVControllerBase::StaticClass();
	check(AIControllerClass);
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AXVEnemyBase::BeginPlay()
{
	Super::BeginPlay();

}
