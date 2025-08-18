#include "System/XVBaseGameMode.h"

#include "AIController.h"
#include "System/XVGameState.h"
#include "Kismet/GameplayStatics.h"
#include "World/SpawnVolume.h"
#include "AI/Character/Base/XVEnemyBase.h"
#include "Blueprint/UserWidget.h"
#include "Character/XVCharacter.h"
#include "GameFramework/PlayerController.h"
#include "System/XVGameInstance.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "EngineUtils.h"
#include "Inventory/Component/InventoryComponent.h"

AXVBaseGameMode::AXVBaseGameMode()
{
	MaxLevel = 5;
}

void AXVBaseGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
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
		GetWorldTimerManager().SetTimerForNextTick([this, GS]()
		{
			if (UGameInstance* GI = GetGameInstance())
			{
				if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
				{
					if (GS->Missions.IsValidIndex(XVGI->CurrentMissionIdx))
					{	
						GS->OnMissionChanged.Broadcast(
							GS->Missions[XVGI->CurrentMissionIdx].MissionTitle,
							GS->Missions[XVGI->CurrentMissionIdx].MissionDescription
							);
						GS->OnEnemyCountChanged.Broadcast(XVGI->TotalKilledEnemyCount);
					}
				}
			}
		});
	}
}

void AXVBaseGameMode::StartGame()
{
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		GS->SpawnedEnemyCount = 0;
		GS->KilledEnemyCount = 0;
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
		UE_LOG(LogTemp, Warning, TEXT("EnemyToSpawn: %d"), EnemyToSpawn);
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
				if (GS->KilledEnemyCount > 0 && GS->KilledEnemyCount >= GS->SpawnAllEnemyCount[XVGI->CurrentLevelIdx])
				{
					GS->BroadcastMission();
					GS->OnEnemyCountChanged.Broadcast(XVGI->TotalKilledEnemyCount);
					EndGame(true);
				}	
			}
		}
	}
}

void AXVBaseGameMode::RespawnPlayer(AController* Controller, float RespawnDelay)
{
	if (!Controller) return;

	APawn* DeadPawn = Controller->GetPawn();
	if (!DeadPawn) return;

	FVector RespawnLocation = DeadPawn->GetActorLocation();
	FRotator RespawnRotation = DeadPawn->GetActorRotation();
	
	Controller->UnPossess();
	if (AXVCharacter* DeadCharacter = Cast<AXVCharacter>(DeadPawn))
	{
		if (UAIPerceptionStimuliSourceComponent* Stimuli = DeadCharacter->FindComponentByClass<UAIPerceptionStimuliSourceComponent>())
		{
			Stimuli->UnregisterFromPerceptionSystem();
		}

		TArray<AActor*> FoundPawns;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), FoundPawns);

		for (AActor* Actor : FoundPawns)
		{
			APawn* Pawn = Cast<APawn>(Actor);
			if (!Pawn) continue;

			AAIController* AICon = Cast<AAIController>(Pawn->GetController());
			if (!AICon) continue;

			if (UAIPerceptionComponent* Perception = AICon->FindComponentByClass<UAIPerceptionComponent>())
			{
				Perception->ForgetActor(DeadCharacter);
			}
		}
		
		DeadCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this, Controller, DeadPawn, RespawnLocation, RespawnRotation]()
	{
		
		if (AXVCharacter* DeadCharacter = Cast<AXVCharacter>(DeadPawn))
		{
			
			DeadCharacter->SetActorLocation(RespawnLocation, false, nullptr, ETeleportType::TeleportPhysics);
			DeadCharacter->SetActorRotation(RespawnRotation);
			

			if (USkeletalMeshComponent* Mesh = DeadCharacter->GetMesh())
			{
				Mesh->SetSimulatePhysics(false);
				Mesh->SetCollisionProfileName(TEXT("Pawn"));
				Mesh->AttachToComponent(
				DeadCharacter->GetCapsuleComponent(),
				FAttachmentTransformRules::KeepRelativeTransform
				);
			}
			
			DeadCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			
			if (UAIPerceptionStimuliSourceComponent* Stimuli = DeadCharacter->FindComponentByClass<UAIPerceptionStimuliSourceComponent>())
			{
				Stimuli->RegisterWithPerceptionSystem();
			}

			GetWorld()->GetTimerManager().SetTimer(
				RespawnSlowHandle,
				[this]()
				{
					if (APlayerController* PC2 = UGameplayStatics::GetPlayerController(this, 0))
					{
						if (AXVCharacter* PlayerChar = Cast<AXVCharacter>(PC2->GetPawn()))
						{
							PlayerChar->CustomTimeDilation = 1.0f;
						}
					}
					
					for (TActorIterator<AXVEnemyBase> It(GetWorld()); It; ++It)
					{
						AXVEnemyBase* Enemy = *It;
						if (IsValid(Enemy))
						{
							Enemy->CustomTimeDilation = 1.0f;
						}
					}
				}, 0.7f,
			false
			);
			
			DeadCharacter->CustomTimeDilation = 1.0f;
			DeadCharacter->CurrentHealth = DeadCharacter->MaxHealth;
			DeadCharacter->bIsDie = false;

			FTimerHandle TimerHandle;
			if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
			{
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, PC]()
			{
				if (PC && PC->PlayerCameraManager)
				{
					PC->PlayerCameraManager->StartCameraFade(
						1.f, 0.f, 0.5f, FLinearColor::Black, false, true);
				}
			}, 1.5f, false);
			}
			
			if (UAnimInstance* AnimInstance = DeadCharacter->GetMesh()->GetAnimInstance())
			{
				AnimInstance->StopAllMontages(0.2f);
			}
			
			DeadCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.0f);
			
			Controller->Possess(DeadCharacter);
		}
	}, RespawnDelay, false);
}

void AXVBaseGameMode::OnWaveTriggered()
{
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		if (GS->IsWaveTriggered) return;

		GS->IsWaveTriggered = true;
		GS->SpawnPatrolEnemyCount = 0;
		FindSpawnVolume();
		SpawnEnemies();
	}
}

void AXVBaseGameMode::EndGame(bool bIsClear)
{
	if (bIsClear)
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
			{
				XVGI->CurrentLevelIdx++;
				if (XVGI->CurrentLevelIdx <	MaxLevel)
				{
					if (AXVGameState* GS = GetGameState<AXVGameState>())
					{
						if (!GS->IsWaveTriggered) return;
						GS->IsWaveTriggered = false;
					}
					if (XVGI->CurrentLevelIdx == 4)
					{
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
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			if (PC && PC->PlayerCameraManager)
			{
				PC->PlayerCameraManager->StartCameraFade(
					0.f,
					1.f,
					0.5f,
					FLinearColor::Black,
					false,
					true
					);
			}
			
			if (AXVCharacter* PlayerChar = Cast<AXVCharacter>(PC->GetPawn()))
			{
				PlayerChar->CustomTimeDilation = 0.1f;
			}
			for (TActorIterator<AXVEnemyBase> It(GetWorld()); It; ++It)
			{
				AXVEnemyBase* Enemy = *It;
				if (IsValid(Enemy))
				{
					Enemy->CustomTimeDilation = 0.1f;
				}
			}
			
			RespawnPlayer(PC, 2.0f);
			if (AXVGameState* GS = GetGameState<AXVGameState>())
			{
				GS->IsWaveTriggered = false;
			}
		}
	}
}


