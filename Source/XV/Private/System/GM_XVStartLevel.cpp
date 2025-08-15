#include "System/GM_XVStartLevel.h"
#include "system/XVGameState.h"
#include "System/XVGameInstance.h"
#include "World/SpawnVolume.h"
#include "AI/Character/Base/XVEnemyBase.h"
#include "Kismet/GameplayStatics.h"

AGM_XVStartLevel::AGM_XVStartLevel()
{
	bWavePending = false;
	Delay = 60.0f;
}

void AGM_XVStartLevel::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
		{
			if (XVGI->WasCinePlay)
			{
				Delay = 0.2f;
			}
		}
	}
	
	GetWorld()->GetTimerManager().ClearTimer(Delayer);

	TWeakObjectPtr<AGM_XVStartLevel> WeakThis = this;

	FTimerDelegate TimerDel;
	TimerDel.BindLambda([WeakThis]()
	{
		if (WeakThis.IsValid())
		{
			WeakThis->StartGame();
		}
	});

	GetWorld()->GetTimerManager().SetTimer(
		Delayer,
		TimerDel,
		Delay,
		false
	);
}

void AGM_XVStartLevel::StartGame()
{
	if (!IsValid(this)) return;
	Super::StartGame();
	
}

void AGM_XVStartLevel::SpawnEnemies()
{
	if (IsOutdoor)
	{
		TArray<AActor*> FoundVolumes;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
	
		if (FoundVolumes.Num() == 0) return;

		if (AXVGameState* GS = GetGameState<AXVGameState>())
		{
			TArray<ASpawnVolume*> ValidVolumes;
			for (AActor* Actor : FoundVolumes)
			{
				if (ASpawnVolume* Volume = Cast<ASpawnVolume>(Actor))
				{
					if (Volume->ActorHasTag("Start_Patrol"))
					{
						ValidVolumes.Add(Volume);
					}
				}
			}
		
			int32 EnemyToSpawn = GS->SpawnPatrolEnemyCount;
			const int32 SpawnVolumeCount = ValidVolumes.Num();
			if (SpawnVolumeCount == 0) return;
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
		}
	}
	
	else
	{
		Super::SpawnEnemies();
	}
}

void AGM_XVStartLevel::OnEnemyKilled()
{
	if (IsOutdoor)
	{
		if (AXVGameState* GS = GetGameState<AXVGameState>())
		{
			GS->KilledEnemyCount++;
			if (GS->KilledEnemyCount > 0 && GS->KilledEnemyCount >= GS->SpawnedEnemyCount)
			{
				IsOutdoor = false;	
				StartGame();
			}	
		}
	}
	else
	{
		OnWaveTriggered();
		if (AXVGameState* GS = GetGameState<AXVGameState>())
		{
			GS->KilledEnemyCount++;
			if (GS->KilledEnemyCount > 0 && GS->KilledEnemyCount >= GS->SpawnedEnemyCount)
			{
				GS->CanActiveArrivalPoint = true;
			}	
		}
	}
}

void AGM_XVStartLevel::OnWaveTriggered()
{
	AXVGameState* GS = GetGameState<AXVGameState>();
	if (!GS) return;

	if (bWavePending || GS->IsWaveTriggered) return;

	bWavePending = true;
	
	GetWorld()->GetTimerManager().SetTimer(
		WaveTriggerDelayHandle,
		this,
		&AGM_XVStartLevel::DelayedWaveTrigger,
		1.0f,
		false
	);
}

void AGM_XVStartLevel::DelayedWaveTrigger()
{
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		GS->IsWaveTriggered = true;
		GS->CanActiveArrivalPoint = false;

		
		Super::SpawnEnemies();
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
				XVGI->IsWaiting = true;
				UGameplayStatics::OpenLevel(GetWorld(), "Level_Laboratory_Demo");
			}
		}
	}
	else
	{
		UGameplayStatics::OpenLevel(GetWorld(), "Showcase");
	}
}


