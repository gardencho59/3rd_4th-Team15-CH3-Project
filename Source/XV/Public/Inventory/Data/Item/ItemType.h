#pragma once

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Health UMETA(DisplayName = "Health"),
	Stamina UMETA(DisplayName = "Stamina"),
	Shield UMETA(DisplayName = "Shield"),
	Bandage UMETA(DisplayName = "Bandage"),
	Armor UMETA(DisplayName = "Armor"),
	Attachment UMETA(DisplayName = "Attachment"),
	AMMO UMETA(DisplayName = "AMMO"),
	None UMETA(DisplayName = "None")
};