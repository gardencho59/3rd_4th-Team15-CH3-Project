#include "System/XVGameState.h"

AXVGameState::AXVGameState()
{
	TimeLimit = 600.0f;
	
	SpawnedEnemyCount = 0;
	KilledEnemyCount = 0;

	SpawnAllEnemyCount = {0, 3, 5, 10, 10};
	SpawnPatrolEnemyCount = 0;
	IsWaveTriggered = false;
	CanActiveArrivalPoint = true;
}