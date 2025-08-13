#include "World/XVDoor.h"
#include "Components/BoxComponent.h"
#include "System/XVGameInstance.h"
#include "System/XVBaseGameMode.h"
#include "System/XVGameState.h"

AXVDoor::AXVDoor()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(SceneRoot);
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
	Super::Tick(DeltaTime);
	
	if (IsOpening)
	{
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

void AXVDoor::OpenDoor()
{
	UE_LOG(LogXVGameMode, Warning, TEXT("OpenDoor() called"));
	TargetPos = ClosedPos + OpenOffset;
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
					
					SetDoorOpening();
					BaseGM->OnWaveTriggered();
				}
			}
		}
	}
}

void AXVDoor::SetDoorOpening()
{
	IsOpening = true;
}