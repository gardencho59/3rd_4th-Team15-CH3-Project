#pragma once

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Consumbale UMETA(DisplayName = "Consumbale"),
	AMMO UMETA(DisplayName = "Ammo"),
	Armor UMETA(DisplayName = "Armor"),
	Attachment UMETA(DisplayName = "Attachments")	
};