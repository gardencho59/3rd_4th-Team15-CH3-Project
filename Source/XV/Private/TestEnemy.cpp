#include "TestEnemy.h"
#include "XVGameMode.h"
#include "XVGameInstance.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"

ATestEnemy::ATestEnemy()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->SetupAttachment(SceneRoot);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	SkeletalMeshComponent->SetupAttachment(SceneRoot);

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ATestEnemy::OnEnemyBeginOverlap);
}

void ATestEnemy::OnEnemyBeginOverlap(
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
		ActivateEnemy(OtherActor);
	}
}
void ATestEnemy::ActivateEnemy(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AXVGameMode* GM = Cast<AXVGameMode>(GetWorld()->GetAuthGameMode()))
		{
			if (UGameInstance* GI = GetGameInstance())
			{
				if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
				{
					GM->OnEnemyKilled();
					Destroy();
				}
			}
		}
	}
}
FName ATestEnemy::GetEnemyType() const
{
	return EnemyType;
}