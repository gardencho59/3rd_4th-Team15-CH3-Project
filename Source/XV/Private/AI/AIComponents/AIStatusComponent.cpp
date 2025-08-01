#include "AI/AIComponents/AIStatusComponent.h"

UAIStatusComponent::UAIStatusComponent()
	: Health(100)
{
	PrimaryComponentTick.bCanEverTick = false;;
}

void UAIStatusComponent::TakeDamage(float Damage)
{
	Health -= Damage;
	if (Health <= 0)
	{
		GetOwner()->Destroy();
	}
}