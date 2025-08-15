#include "World/DoorButton.h"

ADoorButton::ADoorButton()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADoorButton::Unlock()
{
	ChangeMaterial();
}
void ADoorButton::ChangeMaterial()
{
	if (!TargetActor || !NewMaterial) return;

	UStaticMeshComponent* MeshComp = TargetActor->FindComponentByClass<UStaticMeshComponent>();
	if (MeshComp)
	{
		int32 MaterialIndex = 1;
		MeshComp->SetMaterial(MaterialIndex, NewMaterial);
	}
}
