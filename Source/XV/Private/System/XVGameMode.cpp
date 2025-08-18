#include "System/XVGameMode.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void AXVGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetWorld())
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearAllTimersForObject(this);
	}
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC && PC->PlayerCameraManager)
	{
		PC->PlayerCameraManager->StartCameraFade(
			1.0f,
			0.0f,
			1.0f,
			FLinearColor::Black,
			false,
			false
		);
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



	




