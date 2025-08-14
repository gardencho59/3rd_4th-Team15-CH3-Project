#pragma once

#include "CoreMinimal.h"
#include "ItemSFX.generated.h"

USTRUCT(BlueprintType)
struct FItemSFX
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* PickUp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Drop;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Move;
};
