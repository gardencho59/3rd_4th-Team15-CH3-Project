#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

struct FItemData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XV_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();


protected:
	virtual void BeginPlay() override;

	void SetUI(const FItemData ItemData, int32 ItemQuantity, bool IsVisible);

	AActor* TargetActor;

	FTimerHandle InteractionTimerHandle;

public:
	class UInteractionUI* GetUIInstance(AActor* TargetActor);
	
	void InteractionTrace();
	void HandleItemInteract();

	AActor* GetActor();
};
