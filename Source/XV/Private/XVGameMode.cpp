#include "XVGameMode.h"
#include "XVGameState.h"
#include "XVGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "TestEnemy.h"
#include "GameFramework/PlayerController.h"


AXVGameMode::AXVGameMode()
{
	MaxLevel = 3;
}

void AXVGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	FString CurrentMapName = GetWorld()->GetMapName();
	CurrentMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	if (CurrentMapName != "LobbyLevel")	
	{
		StartGame();
	}
}

void AXVGameMode::StartGame()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
		{
			if (XVGI->IsWaiting)	
			{
				XVGI->IsWaiting = false;
			
				if (LevelNames.IsValidIndex(XVGI->CurrentLevelIdx))
				{
					UE_LOG(LogTemp, Warning, TEXT("OpenLevel: %s"), *LevelNames[XVGI->CurrentLevelIdx].ToString());
					UGameplayStatics::OpenLevel(GetWorld(), LevelNames[XVGI->CurrentLevelIdx]);
					return;
				}
			}
		}
	}
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Level Starts!"));
		GS->SpawnedEnemyCount = 0;
		GS->KilledEnemyCount = 0;
	}

	SpawnEnemies();

	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		GetWorldTimerManager().SetTimer(
			XVGameTimerHandle,
			this,
			&AXVGameMode::OnTimeLimitExceeded,
			GS->TimeLimit,
			false);
	}
}

void AXVGameMode::SpawnEnemies() const
{
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
	
	if (FoundVolumes.Num() == 0) return;

	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
			{
				const int32 EnemyToSpawn = (XVGI->CurrentLevelIdx + 5);
				for (int32 i = 0; i < EnemyToSpawn; i++)
				{
					if (ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]))
					{
						AActor* SpawnActor = SpawnVolume->SpawnRandomEnemy();
						if (SpawnActor && SpawnActor->IsA(ATestEnemy::StaticClass()))
						{
							GS->SpawnedEnemyCount++;
						}
					}
				}
			}
		}
	}
}

void AXVGameMode::OnEnemyKilled()
{
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		GS->KilledEnemyCount++;
		if (GS->KilledEnemyCount > 0 && GS->KilledEnemyCount >= GS->SpawnedEnemyCount)
		{
			EndGame(true);
		}
	}
}

void AXVGameMode::OnTimeLimitExceeded()
{
	EndGame(false);
}

void AXVGameMode::EndGame(bool bIsClear)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->SetPause(true);	
	}
	
	if (bIsClear)
	{
		//Slow Motion
		//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.2);
		
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
			{
				if (XVGI->CurrentLevelIdx <	MaxLevel)
				{
					UE_LOG(LogTemp, Warning, TEXT("Level %d Clear!"), XVGI->CurrentLevelIdx + 1);
					XVGI->CurrentLevelIdx++;
					XVGI->IsWaiting = true;
					UGameplayStatics::OpenLevel(GetWorld(), "LobbyLevel");
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
		UE_LOG(LogTemp, Warning, TEXT("Game Over!"));
	}
}



	




