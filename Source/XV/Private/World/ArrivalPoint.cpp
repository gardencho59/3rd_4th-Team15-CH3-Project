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
				Destroy();
			}
		}
	}
}

void AArrivalPoint::ActivateArrivalPoint(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AXVGameState* GS = GetWorld() ? GetWorld()->GetGameState<AXVGameState>() : nullptr)
		{
			GS->BroadcastMission();
		}
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
			{
				XVGI->CurrentLevelIdx++;
			}
		}
		if (AXVBaseGameMode* GM = Cast<AXVBaseGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->EndGame(true);
		}
	}
}