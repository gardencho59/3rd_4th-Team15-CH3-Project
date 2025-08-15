// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/InteractableItem.h"
#include "ShieldItem.generated.h"

/**
 * 
 */
UCLASS()
class XV_API AShieldItem : public AInteractableItem
{
	GENERATED_BODY()
	AShieldItem();
	
	// 아이템 인터페이스
	virtual void UseItem() override;
	void FinishItem();
	
	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual float GetItemTime() override; 	// 아이템 지속 시간 반환
	UFUNCTION(BlueprintCallable, Category = "Item")
	float GetItemRemainTime(); 	// 아이템 남은 시간 반환

protected:
	UPROPERTY(EditAnywhere, Category = "Item")
	float Shield;

	FTimerHandle ShieldTimerHandle;		
};
