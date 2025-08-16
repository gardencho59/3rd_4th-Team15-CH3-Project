#pragma once

UENUM(BlueprintType)
enum class EAttachmentType : uint8
{
	None UMETA(DisplayName = "None"),
	Silencer UMETA(DisplayName = "Silencer"),
	ExtendedMag UMETA(DisplayName = "ExtendedMag"),
};