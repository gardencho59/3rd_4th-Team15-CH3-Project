#include "World/XVDoor.h"
#include "World/DoorButton.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
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
			Destroy();
		}
	}
}

void AXVDoor::OpenDoor(UBoxComponent* TriggerBox)
{
	if (!IsOpening)
	{
		if (ADoorButton* DoorButtonRef = Cast<ADoorButton>(UGameplayStatics::GetActorOfClass(GetWorld(), ADoorButton::StaticClass())))
		{
			if (ExtraBox)
			{
				if (TriggerBox == ExtraBox)
				{
					if (AXVGameState* GS = GetWorld()->GetGameState<AXVGameState>())
					{
						if (GS->CurrentMissionIdx != 2) return;
						if (DoorButtonRef->IsLocked)
						{
							DoorButtonRef->Unlock();
							DoorButtonRef->IsLocked = false;
							ExtraBox->DestroyComponent();
							ExtraBox = nullptr;
						}
					}
				}
			}
			if (TriggerBox == DoorBox && DoorButtonRef->IsLocked) return;
		
			if (UGameInstance* GI = GetGameInstance())
			{
				if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
				{
					if (AXVBaseGameMode* BaseGM = GetWorld()->GetAuthGameMode<AXVBaseGameMode>())
					{
						if (XVGI->CurrentLevelIdx == 3)
						{
							if (AXVGameState* GS = GetWorld()->GetGameState<AXVGameState>())
							{
								if (GS->IsWaveTriggered) return;
							
								GS->BroadcastMission();
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
										DoorBox->DestroyComponent();
										DoorButtonRef->IsLocked = true;
										break;

									case 2:
										GS->BroadcastMission();
										break;
								
									case 3:
										if (!IsValid(ExtraBox))
										{
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
}

void AXVDoor::SetDoorOpening()
{
	IsOpening = true;
	TargetPos = ClosedPos + OpenOffset;
	if (AXVGameState* GS = GetWorld()->GetGameState<AXVGameState>())
	{
		if (GS->CurrentMissionIdx == 7)
		{
			if (AXVBaseGameMode* GM = GetWorld()->GetAuthGameMode<AXVBaseGameMode>())
			{
				GM->StartGame();
			}
		}
	}
	
}