// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/InteractiveItem.h"
#include "BandageItem.generated.h"

UCLASS()
class XV_API ABandageItem : public AInteractiveItem
{
	GENERATED_BODY()
	ABandageItem();
	
	// 아이템 인터페이스
	virtual void UseItem() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Item")
	float HealAmount; // 체력 회복 수치
};
