#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "ItemSFX.h"
#include "ItemType.h"
#include "ItemData.generated.h"

class AInteractiveItem;

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	// 아이템 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;

	// 아이템 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType ItemType;

	// 아이템 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemIcon;

	// 아이템 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AInteractiveItem> ItemClass;

	// 아이템 사운드 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemSFX ItemSFX;

	// 아이템 별 최대 수량
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SlotQuantity;
};
