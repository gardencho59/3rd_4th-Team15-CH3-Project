#include "System/XVGameMode.h"
#include "System/XVGameState.h"
#include "System/XVGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"


AXVGameMode::AXVGameMode()
{
	MaxLevel = 5;
}

void AXVGameMode::BeginPlay()
{
	Super::BeginPlay();
	StartGame();
}

void AXVGameMode::StartGame()
{
	Super::StartGame();
}

void AXVGameMode::FindSpawnVolume()
{
	UE_LOG(LogTemp, Warning, TEXT("FindSpawnVolume!"));
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
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		if (GS->IsWaveTriggered) return;

		UE_LOG(LogTemp, Warning, TEXT("WaveTriggered!"));
		GS->IsWaveTriggered = true;
		FindSpawnVolume();
		SpawnEnemies();
	}
}

void AXVGameMode::EndGame(bool bIsClear)
{
	if (GetWorld())
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearAllTimersForObject(this);
	}
	
	Super::EndGame(bIsClear);
	
	if (bIsClear)
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
			{
				if (XVGI->CurrentLevelIdx <	MaxLevel)
				{
					XVGI->CurrentLevelIdx++;
					if (AXVGameState* GS = GetGameState<AXVGameState>())
					{
						if (!GS->IsWaveTriggered) return;
						GS->IsWaveTriggered = false;
						StartGame();
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Game Clear!"));
				}
			}
		}
	}
	else
	{
		//Slow Motion
		//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.2);
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
			{
				XVGI->CurrentLevelIdx = 1;
				UGameplayStatics::OpenLevel(GetWorld(), "Level_Laboratory_Demo");
			}
		}
	}
}



	




