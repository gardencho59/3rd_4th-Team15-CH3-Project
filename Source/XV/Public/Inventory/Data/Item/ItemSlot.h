#pragma once

#include "CoreMinimal.h"
#include "ItemType.h"
#include "ItemSlot.generated.h"

USTRUCT(BlueprintType)
struct FItemSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ItemQuantity;
};
