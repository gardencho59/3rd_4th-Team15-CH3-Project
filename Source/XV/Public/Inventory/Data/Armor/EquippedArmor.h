#pragma once

#include "CoreMinimal.h"
#include "ArmorData.h"
#include "EquippedArmor.generated.h"

USTRUCT(BlueprintType)
struct FEquippedArmor : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY()
	FArmorData Helmet;
	
	UPROPERTY()
	FArmorData Vest;
};