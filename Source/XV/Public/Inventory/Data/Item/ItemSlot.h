#pragma once

#include "CoreMinimal.h"
#include "ItemType.h"
#include "ItemSlot.generated.h"

USTRUCT(BlueprintType)
struct FItemSlot
{
	GENERATED_BODY()

	UPROPERTY()
	FName ItemID;
	
	UPROPERTY()
	EItemType ItemType;

	UPROPERTY()
	float ItemQuantity;
};
