#include "StarterActor.h"
#include "Components/BoxComponent.h"
#include "XVGameMode.h"
#include "XVGameInstance.h"

AStarterActor::AStarterActor()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->SetupAttachment(SceneRoot);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetupAttachment(SceneRoot);
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AStarterActor::OnStarterBeginOverlap);
}

void AStarterActor::OnStarterBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,	
		int32 OtherBodyIdx,
		bool bFromSweep,
		const FHitResult& SweepResult
	)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		ActivateStarter(OtherActor);
	}
}
void AStarterActor::ActivateStarter(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AXVGameMode* GM = Cast<AXVGameMode>(GetWorld()->GetAuthGameMode()))
		{
			if (UGameInstance* GI = GetGameInstance())
			{
				if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
				{
					if (XVGI->IsWaiting)
					{
						GM->StartGame();
						Destroy();
					}
				}
			}
		}
	}
}