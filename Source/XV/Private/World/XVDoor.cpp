#include "World/XVDoor.h"
#include "Components/BoxComponent.h"
#include "System/XVGameInstance.h"
#include "System/XVBaseGameMode.h"
#include "System/XVGameState.h"

AXVDoor::AXVDoor()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	DoorBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorBox"));
	DoorBox->SetupAttachment(SceneRoot);
	ExtraBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ExtraBox"));
	ExtraBox->SetupAttachment(SceneRoot);
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(SceneRoot);
	
	OpenOffset = FVector(0.f, 0.f, 235.f);
	IsOpening = false;
	OpenTime = 30.f;

	PrimaryActorTick.bCanEverTick = true;
}

void AXVDoor::BeginPlay()
{
	Super::BeginPlay();
	
	ClosedPos = Door->GetRelativeLocation();
}

void AXVDoor::Tick(float DeltaTime)
{
	if (IsOpening)
	{
		Super::Tick(DeltaTime);
		
		const float Speed = 1.f;
		Door->SetRelativeLocation(FMath::VInterpTo(Door->GetRelativeLocation(), TargetPos, DeltaTime, Speed));
	
		const float Threshold = 15.0f;
		if (FVector::Dist(Door->GetRelativeLocation(), TargetPos) < Threshold)
		{	
			UE_LOG(LogTemp, Warning, TEXT("Door Opened"));
			Destroy();
		}
	}
}

void AXVDoor::OpenDoor(UBoxComponent* TriggerBox)
{
	UE_LOG(LogTemp, Warning, TEXT("OpenDoor() Called"));
	UE_LOG(LogTemp, Warning, TEXT("IsOpening: %s"), IsOpening ? TEXT("true") : TEXT("false"));
	if (!IsOpening)
	{
		UE_LOG(LogTemp, Warning, TEXT("a"));
		if (ExtraBox)
		{
			if (TriggerBox == ExtraBox)
			{
				if (AXVGameState* GS = GetWorld()->GetGameState<AXVGameState>())
				{
					if (GS->CurrentMissionIdx != 2)
					{
						UE_LOG(LogTemp, Warning, TEXT("Cannot Unlock Door"));
						return;
					}
				}
				UE_LOG(LogTemp, Warning, TEXT("Door UnLocked"));
				IsLocked = false;
				ExtraBox->DestroyComponent();
                ExtraBox = nullptr;
			}
		}
		if (TriggerBox == DoorBox && IsLocked)
		{
			UE_LOG(LogTemp, Warning, TEXT("Door is Locked"));
			return;
		}
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
			{
				UE_LOG(LogXVGameMode, Warning, TEXT("CurrentLevelIdx: %d"), XVGI->CurrentLevelIdx);
				if (AXVBaseGameMode* BaseGM = GetWorld()->GetAuthGameMode<AXVBaseGameMode>())
				{
					if (XVGI->CurrentLevelIdx == 3)
					{
						if (AXVGameState* GS = GetWorld()->GetGameState<AXVGameState>())
						{
							if (GS->IsWaveTriggered) return;
					
							GS->IsWaveTriggered = true;
							BaseGM->StartGame();
							GetWorldTimerManager().SetTimer(
								DoorOpenTimerInWave,
								this,
								&AXVDoor::SetDoorOpening,
								OpenTime,
								false
								);
						}
					}
					else
					{
						if (AXVGameState* GS = GetWorld()->GetGameState<AXVGameState>())
						{
							if (AXVBaseGameMode* GM = GetWorld()->GetAuthGameMode<AXVBaseGameMode>())
							{
								switch (GS->CurrentMissionIdx)
								{
								case 0:
									GM->StartGame();
									SetDoorOpening();
									GS->BroadcastMission();
									break;

								case 2:
									GS->BroadcastMission();
									break;
								
								case 3:
									if (!IsValid(ExtraBox))
									{
										UE_LOG(LogTemp, Warning, TEXT("Case 3"));
										GM->StartGame();
										SetDoorOpening();
										GS->BroadcastMission();
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void AXVDoor::SetDoorOpening()
{
	IsOpening = true;
	TargetPos = ClosedPos + OpenOffset;
}