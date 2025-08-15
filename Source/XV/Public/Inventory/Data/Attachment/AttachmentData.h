#pragma once

#include "CoreMinimal.h"
#include "AttachmentType.h"
#include "AttachmentData.generated.h"

USTRUCT(BlueprintType)
struct FAttachmentData : public FTableRowBase
{
	GENERATED_BODY()
	
	// 부착물 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AttachmentName;

	// 부착물 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttachmentType AttachmentType;

	// 부착물 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* AttachmentMesh;
};