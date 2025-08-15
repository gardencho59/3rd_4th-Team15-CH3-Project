#include "System/XVBaseGameMode.h"

#include "VisualizeTexture.h"
#include "System/XVGameState.h"
#include "Kismet/GameplayStatics.h"
#include "World/SpawnVolume.h"
#include "AI/Character/Base/XVEnemyBase.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "System/XVGameInstance.h"


void AXVBaseGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AXVBaseGameMode::StartGame()
{
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		GS->SpawnedEnemyCount = 0;
		GS->KilledEnemyCount = 0;
		if (GS->GameUIClass)
		{
			if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
			{
				if (UUserWidget* HUD = CreateWidget<UUserWidget>(PC, GS->GameUIClass))
				{
					HUD->AddToViewport();
				}
			}
		}
		GetWorldTimerManager().SetTimerForNextTick([GS]()
		{
			if (GS->MissionTexts1.IsValidIndex(0)) GS->OnMissionChanged.Broadcast(GS->MissionTexts1[GS->CurrentMissionIdx]);
		});
	}
	FindSpawnVolume();
	SpawnEnemies();
}

void AXVBaseGameMode::FindSpawnVolume()
{
	ActiveSpawnVolumes.Empty();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
		{
			FString LevelTag = FString::Printf(TEXT("Level%d"), XVGI->CurrentLevelIdx);

			TArray<AActor*> FoundVolumes;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

			for (AActor* Actor : FoundVolumes)
			{
				if (!Actor) continue;

				bool bHasPatrolOrWave = Actor->Tags.Contains("Patrol") || Actor->Tags.Contains("Wave");
				bool bHasLevelTag = Actor->Tags.Contains(LevelTag);
				
				if (bHasPatrolOrWave && bHasLevelTag)
				{
					if (ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(Actor))
					{
						ActiveSpawnVolumes.Add(SpawnVolume);
					}
				}
			}
		}
	}
}
	
void AXVBaseGameMode::SpawnEnemies()
{
	if (ActiveSpawnVolumes.IsEmpty())
	{
		return;
	}
	
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		TArray<ASpawnVolume*> ValidVolumes;
		
		for (ASpawnVolume* Volume : ActiveSpawnVolumes)
		{
			if (!Volume) continue;

			FString TagString;
			for (const auto& Tag : Volume->Tags)
			{
				TagString += Tag.ToString() + TEXT(" ");
			}
			
			if (!GS->IsWaveTriggered && Volume->ActorHasTag("Patrol"))
			{
				ValidVolumes.Add(Volume);
				GS->SpawnPatrolEnemyCount++;
			}
			else if (GS->IsWaveTriggered && Volume->ActorHasTag("Wave"))
			{
				ValidVolumes.Add(Volume);
			}
		}
		
		int32 EnemyToSpawn = 0;
		if (!GS->IsWaveTriggered) EnemyToSpawn = GS->SpawnPatrolEnemyCount;
		else
		{
			if (UGameInstance* GI = GetGameInstance())
			{
				if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
				{
					EnemyToSpawn = GS->SpawnAllEnemyCount[XVGI->CurrentLevelIdx] - GS->SpawnPatrolEnemyCount;
				}
			}
		}
		
		const int32 SpawnVolumeCount = ValidVolumes.Num();
		if (SpawnVolumeCount == 0)
		{
			return;
		}
		for (int32 i = 0; i < EnemyToSpawn; i++)
		{
			ASpawnVolume* SpawnVolume = ValidVolumes[i % SpawnVolumeCount];
			if (!SpawnVolume) continue;
		
			
			AActor* SpawnActor = SpawnVolume->SpawnRandomEnemy();
			if (SpawnActor && SpawnActor->IsA(AXVEnemyBase::StaticClass()))
			{
				GS->SpawnedEnemyCount++;
			}
		}
		if (GS->SpawnedEnemyCount > 0)
		{
			GetWorldTimerManager().SetTimerForNextTick([GS]()
			{
				GS->OnEnemyCountChanged.Broadcast(GS->SpawnedEnemyCount - GS->KilledEnemyCount);
			});
		}
	}
}

void AXVBaseGameMode::OnEnemyKilled()
{
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		GS->KilledEnemyCount++;
		GS->EnemyKilled();
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
			{
				XVGI->TotalEnemyKilled++;
				if (GS->KilledEnemyCount > 0 && GS->KilledEnemyCount >= GS->SpawnAllEnemyCount[XVGI->CurrentLevelIdx])
				{
					GS->BroadcastMission();
					EndGame(true);
				}	
			}
		}
	}
}

void AXVBaseGameMode::OnWaveTriggered()
{
	
}

void AXVBaseGameMode::EndGame(bool bIsClear)
{
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		GS->SpawnPatrolEnemyCount = 0;
	}
}


