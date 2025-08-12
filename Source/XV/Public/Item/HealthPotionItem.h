#pragma once

#include "CoreMinimal.h"
#include "Item/InteractableItem.h"
#include "HealthPotionItem.generated.h"


UCLASS()
class XV_API AHealthPotionItem : public AInteractableItem
{
	GENERATED_BODY()
public:
	AHealthPotionItem();
	virtual void BeginPlay() override;
	void ResetHealthPotion();
	void StartUse();
	void StopUse();
	void HealTick();
	void UseItem();
protected:
	void Interact() override;
	
private:
	float TotalHealAmount; 
	float HealDuration;
	float HealPerTick;
	float RemainingHealAmount;
	FTimerHandle HealTimerHandle;
};
