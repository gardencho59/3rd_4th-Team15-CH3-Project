#include "AI/AIComponents/AIStatusComponent.h"

#include <System/XVBaseGameMode.h>

UAIStatusComponent::UAIStatusComponent()
	: Health(100)
	, AttackDamage(10)
{
	PrimaryComponentTick.bCanEverTick = false;;
}

void UAIStatusComponent::TakeDamage(float Damage)
{
	Health -= Damage;
	if (Health <= 0 && EnemyKilled == false)
	{
		Health = 0;
		EnemyKilled = true;
	}

	// OnEnemyKilled 호출
	if (EnemyKilled == true)
	{
		if(AXVBaseGameMode* BaseGameMode = GetWorld()->GetAuthGameMode<AXVBaseGameMode>())
		{
			UE_LOG(LogTemp, Warning, TEXT("OnEnemyKilled"));
			BaseGameMode->OnEnemyKilled();
		}
	}
		
}