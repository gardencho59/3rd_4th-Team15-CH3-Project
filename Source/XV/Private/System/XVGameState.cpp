#include "System/XVGameState.h"
#include "Blueprint/UserWidget.h"

AXVGameState::AXVGameState()
{
	TimeLimit = 600.0f;
	
	SpawnedEnemyCount = 0;
	KilledEnemyCount = 0;

	SpawnAllEnemyCount = {15, 3, 5, 10, 10};
	SpawnPatrolEnemyCount = 0;
	IsWaveTriggered = false;
	CanActiveArrivalPoint = false;
}

void AXVGameState::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (GameUIClass)
		{
			if (UUserWidget* GameUI = CreateWidget<UUserWidget>(PC, GameUIClass))
			{
				GameUI->AddToViewport();
			}
		}
	}
}
