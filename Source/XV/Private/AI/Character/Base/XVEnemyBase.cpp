#include "AI/Character/Base/XVEnemyBase.h"

// 추가됨
#include "AI/System/AIController/Base/XVControllerBase.h"

AXVEnemyBase::AXVEnemyBase()
{
	AIControllerClass = AXVControllerBase::StaticClass();
	check(AIControllerClass);
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}
