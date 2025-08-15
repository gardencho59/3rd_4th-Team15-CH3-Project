#pragma once

#include "CoreMinimal.h"
#include "AttachmentData.h"
#include "EquippedAttachment.generated.h"

USTRUCT(BlueprintType)
struct FEquippedAttachment : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY()
	FAttachmentData Silencer;
	
	UPROPERTY()
	FAttachmentData ExtendedMag;
};