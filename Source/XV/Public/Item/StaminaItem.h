// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/InteractableItem.h"
#include "StaminaItem.generated.h"

/**
 * 
 */
UCLASS()
class XV_API AStaminaItem : public AInteractableItem
{
	GENERATED_BODY()

public:
	AStaminaItem();
	
	// 아이템 인터페이스
	virtual void UseItem() override;
	void FinishItem();


protected:
	UPROPERTY(EditAnywhere, Category = "Item")
	float Speed;
	UPROPERTY(EditAnywhere, Category = "Item")
	float StaminaTime;
	FTimerHandle StaminaTimerHandle;	
};
