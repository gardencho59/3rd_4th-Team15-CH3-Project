// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/InteractiveItem.h"
#include "ShieldItem.generated.h"

/**
 * 
 */
UCLASS()
class XV_API AShieldItem : public AInteractiveItem
{
	GENERATED_BODY()
	AShieldItem();
	
	// 아이템 인터페이스
	virtual void UseItem() override;
	void FinishItem();

protected:
	UPROPERTY(EditAnywhere, Category = "Item")
	float Shield;
};
