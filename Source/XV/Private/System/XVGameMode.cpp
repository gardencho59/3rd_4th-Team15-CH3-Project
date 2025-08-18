#include "System/XVGameMode.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Character/XVCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "System/XVGameInstance.h"

void AXVGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (AXVCharacter* XVChar = Cast<AXVCharacter>(PC->GetPawn()))
		{
			if (UInventoryComponent* Inventory = XVChar->FindComponentByClass<UInventoryComponent>())
			{
				if (UGameInstance* GI = GetGameInstance())
				{
					if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
					{
						XVGI->LoadInventory(Inventory);
						Inventory->ToggleInventory();
						Inventory->ToggleInventory();
						XVChar->SetHealth(XVGI->SavedHealth);
					}
				}
			}
		}
	}
	
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

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
		{
			if (bIsClear && XVGI->CurrentLevelIdx == 5)
			{
				GameClear();
			}
		}
	}
	
}


	




