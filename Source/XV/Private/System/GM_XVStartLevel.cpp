#include "System/GM_XVStartLevel.h"
#include "system/XVGameState.h"
#include "System/XVGameInstance.h"
#include "Kismet/GameplayStatics.h"

AGM_XVStartLevel::AGM_XVStartLevel()
{
	bWavePending = false;
	Delay = 60.0f;
}

void AGM_XVStartLevel::BeginPlay()
{
	Super::BeginPlay();
}

void AGM_XVStartLevel::StartGame()
{
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		GS->KilledEnemyCount = 0;
	}
}

void AGM_XVStartLevel::OnEnemyKilled()
{
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		GS->KilledEnemyCount++;
		GS->EnemyKilled();
		
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
			{
				GS->OnEnemyCountChanged.Broadcast(XVGI->TotalKilledEnemyCount);
				
				if (GS->KilledEnemyCount > 0 && GS->KilledEnemyCount >= GS->SpawnAllEnemyCount[XVGI->CurrentLevelIdx])
				{
					if (GS->CanActiveArrivalPoint) return;
					
					GS->BroadcastMission();
					GS->CanActiveArrivalPoint = true;
				}	
			}
		}
	}
}

void AGM_XVStartLevel::OnWaveTriggered()
{
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		if (GS->IsWaveTriggered) return;
		GS->IsWaveTriggered = true;
	}
}

void AGM_XVStartLevel::EndGame(bool bIsClear)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}
	
	if (bIsClear)
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
			{
				UGameplayStatics::OpenLevel(GetWorld(), "Level_Laboratory_Demo");
			}
		}
	}
	else
	{
		Super::EndGame(bIsClear);
	}
}


