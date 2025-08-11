#include "World/ArrivalPoint.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "System/XVBaseGameMode.h"
#include "System/XVGameInstance.h"
#include "System/XVGameState.h"

AArrivalPoint::AArrivalPoint()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->SetupAttachment(SceneRoot);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetupAttachment(SceneRoot);
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AArrivalPoint::OnArrivalPointBeginOverlap);
}

void AArrivalPoint::OnArrivalPointBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,	
		int32 OtherBodyIdx,
		bool bFromSweep,
		const FHitResult& SweepResult
	)
{
	if (AXVGameState* GS = GetWorld() ? GetWorld()->GetGameState<AXVGameState>() : nullptr)
	{
		if (GS->CanActiveArrivalPoint)
		{
			if (OtherActor && OtherActor->ActorHasTag("Player"))
			{
				ActivateArrivalPoint(OtherActor);
			}
		}
	}
}

void AArrivalPoint::ActivateArrivalPoint(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AXVBaseGameMode* BaseGM = Cast<AXVBaseGameMode>(GetWorld()->GetAuthGameMode()))
		{
			if (UGameInstance* GI = GetGameInstance())
			{
				if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
				{
					if (!XVGI->IsWaiting)
					{
						BaseGM->EndGame(true);
						Destroy();
					}
					else
					{
						if (LevelNames.IsValidIndex(XVGI->CurrentLevelIdx))
						{
							XVGI->IsWaiting = false;
							UE_LOG(LogTemp, Warning, TEXT("OpenLevel: %s"), *LevelNames[XVGI->CurrentLevelIdx].ToString());
							UGameplayStatics::OpenLevel(GetWorld(), LevelNames[XVGI->CurrentLevelIdx]);
						}
					}
				}
			}
			
			
		}
	}
}