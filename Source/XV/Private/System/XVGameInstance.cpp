#include "System/XVGameInstance.h"

UXVGameInstance::UXVGameInstance()
{
	CurrentLevelIdx = 0;
	CurrentMissionIdx = 0;
	IsWaiting = true;
	WasCinePlay = false;
	IsOutdoor = false;
	TotalKilledEnemyCount = 0;
}