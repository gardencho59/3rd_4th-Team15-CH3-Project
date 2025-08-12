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
	virtual void UseItem() override;

	float GetChargeTime() const;
	float GetChargeRemainTime() const;
	float GetChargeCurrentTime() const;
	
	void StartUse();
	void StopUse();
	UFUNCTION()
	void FinishUse(); // 회복 실행
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Potion")
	float ChargeTime; // 사용까지 필요한 시간 (초)
	UPROPERTY(EditDefaultsOnly, Category="Potion")
	float HealAmount; // 회복량

	FTimerHandle HealTimerHandle;

	virtual void Interact() override;

	
private:
	bool bIsUsing;
};
