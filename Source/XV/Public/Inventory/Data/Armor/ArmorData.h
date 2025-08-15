#pragma once

#include "CoreMinimal.h"
#include "ArmorType.h"
#include "ArmorData.generated.h"

USTRUCT(BlueprintType)
struct FArmorData : public FTableRowBase
{
	GENERATED_BODY()

	// 방어구 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ArmorName;

	// 방어구 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EArmorType ArmorType;

	// 방어구 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* ArmorMesh;

	// 방어구 레벨
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ArmorLevel;

	// 체력 증가량
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ArmorHealth;
};