#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XV_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();


protected:
	virtual void BeginPlay() override;

	AActor* TargetActor;

	FTimerHandle InteractionTimerHandle;

public:
	class UInteractionUI* GetUIInstance();
	
	void InteractionTrace();
	void HandleItemInteract();

	AActor* GetActor();
};
