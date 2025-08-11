#include "System/XVGameMode.h"
#include "System/XVGameState.h"
#include "System/XVGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"


AXVGameMode::AXVGameMode()
{
	MaxLevel = 3;
}

void AXVGameMode::BeginPlay()
{
	Super::BeginPlay();
	StartGame();
}

void AXVGameMode::StartGame()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
		{
			if (!XVGI->IsWaiting)	
			{
				Super::StartGame();
				if (!GetWorldTimerManager().IsTimerActive(XVGameTimerHandle))
				{
					if (AXVGameState* GS = GetGameState<AXVGameState>())
					{
						TWeakObjectPtr<AXVGameMode> WeakThis = this;

						FTimerDelegate TimerDel;
						TimerDel.BindLambda([WeakThis]()
						{
							if (WeakThis.IsValid())
							{
								WeakThis->OnTimeLimitExceeded();
							}
						});

						GetWorldTimerManager().SetTimer(
							XVGameTimerHandle,
							TimerDel,
							GS->TimeLimit,
							false
						);
					}
				}
			}
		}
	}
}
	
void AXVGameMode::SpawnEnemies()
{
	Super::SpawnEnemies();
}

void AXVGameMode::OnEnemyKilled()
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

void AXVGameMode::OnWaveTriggered()
{
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		GS->CanActiveArrivalPoint = false;
		if (GS->IsWaveTriggered) return;
		
		GS->IsWaveTriggered = true;
		SpawnEnemies();
	}
}

void AXVGameMode::OnTimeLimitExceeded()
{
	Super::OnTimeLimitExceeded();
}

void AXVGameMode::EndGame(bool bIsClear)
{
	if (GetWorld())
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearAllTimersForObject(this);
		
		if (TimerManager.IsTimerActive(XVGameTimerHandle))
		{
			TimerManager.ClearTimer(XVGameTimerHandle);
		}
	}
	
	Super::EndGame(bIsClear);
	
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
					XVGI->CurrentLevelIdx++;
					XVGI->IsWaiting = true;
					UGameplayStatics::OpenLevel(GetWorld(), "BaseLevel");
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
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
			{
				XVGI->IsWaiting = true;
				UGameplayStatics::OpenLevel(GetWorld(), "BaseLevel");
			}
		}
	}
}



	




