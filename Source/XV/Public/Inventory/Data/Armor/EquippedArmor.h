#pragma once

#include "CoreMinimal.h"
#include "ArmorData.h"
#include "EquippedArmor.generated.h"

USTRUCT(BlueprintType)
struct FEquippedArmor : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FArmorData Helmet;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FArmorData Vest;
};