#include "System/XVGameInstance.h"
#include "Inventory/Component/InventoryComponent.h"

UXVGameInstance::UXVGameInstance()
{
	CurrentLevelIdx = 0;
	CurrentMissionIdx = 0;
	WasCinePlay = false;
	TotalKilledEnemyCount = 0;
}

void UXVGameInstance::SaveInventory(class UInventoryComponent* Inventory)
{
	if (!Inventory) return;
	SavedItemSlots = Inventory->GetItemSlots();
	SavedEquippedArmor = Inventory->GetEquippedArmor();
	SavedRifleAttachment = Inventory->GetEquippedRifleAttachment();
	SavedPistolAttachment = Inventory->GetEquippedPistolAttachment();
}

void UXVGameInstance::LoadInventory(class UInventoryComponent* Inventory)
{
	if (!Inventory) return;
	Inventory->SetItemSlots(SavedItemSlots);
	Inventory->SetEquippedArmor(SavedEquippedArmor);
	Inventory->SetEquippedRifleAttachment(SavedRifleAttachment);
	Inventory->SetEquippedPistolAttachment(SavedPistolAttachment);
}