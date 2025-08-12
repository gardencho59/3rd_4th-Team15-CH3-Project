#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "ItemSFX.h"
#include "ItemData.generated.h"

class AInteractableItem;

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AInteractableItem> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemSFX ItemSFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ItemQuantity;
};
