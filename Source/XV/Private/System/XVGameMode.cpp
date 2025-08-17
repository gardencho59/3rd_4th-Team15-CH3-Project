#include "System/XVGameMode.h"
#include "GameFramework/PlayerController.h"

void AXVGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetWorld())
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearAllTimersForObject(this);
	}
}

void AXVGameMode::StartGame()
{
	Super::StartGame();
}

void AXVGameMode::FindSpawnVolume()
{
	Super::FindSpawnVolume();
}

void AXVGameMode::SpawnEnemies()
{
	Super::SpawnEnemies();
}

void AXVGameMode::OnEnemyKilled()
{
	Super::OnEnemyKilled();
}

void AXVGameMode::OnWaveTriggered()
{
	Super::OnWaveTriggered();
}

void AXVGameMode::EndGame(bool bIsClear)
{
	Super::EndGame(bIsClear);
}



	




