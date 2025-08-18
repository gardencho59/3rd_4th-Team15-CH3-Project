#include "System/GM_XVStartLevel.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Character/XVCharacter.h"
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

void AGM_XVStartLevel::EndGame(bool bIsClear)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}
	
	if (bIsClear)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC && PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->StartCameraFade(
				0.0f,
				1.0f,
				1.0f,
				FLinearColor::Black,
				false,
				true
			);
			if (AXVCharacter* XVChar = Cast<AXVCharacter>(PC->GetPawn()))
			{
				if (UInventoryComponent* Inventory = XVChar->FindComponentByClass<UInventoryComponent>())
				{
					if (UGameInstance* GI = GetGameInstance())
					{
						if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
						{
							XVGI->SaveInventory(Inventory);
							XVGI->SavedHealth = XVChar->GetHealth();
						}
					}
				}
			}
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [this]()
			{
				UGameplayStatics::OpenLevel(GetWorld(), "LoadingMenu");
			}, 1.05f, false);
		}
	}
	else
	{
		Super::EndGame(bIsClear);
	}
}


