#pragma once

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Health UMETA(DisplayName = "Health"),
	Stamina UMETA(DisplayName = "Stamina"),
	Shield UMETA(DisplayName = "Shield"),
	Armor UMETA(DisplayName = "Armor"),
	Attachment UMETA(DisplayName = "Attachment"),
	AMMORifle UMETA(DisplayName = "AMMORifle"),
	AMMOPistol UMETA(DisplayName = "AMMOPistol"),
	None UMETA(DisplayName = "None")
};