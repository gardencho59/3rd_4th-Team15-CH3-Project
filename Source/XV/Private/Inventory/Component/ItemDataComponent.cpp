#include "Inventory/Component/ItemDataComponent.h"
#include "Inventory/Data/ItemData.h"
#include "Inventory/Component/InventoryComponent.h"

UItemDataComponent::UItemDataComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FItemData* UItemDataComponent::GetItemData()
{
	if (!ItemDataHandle.DataTable)
	{
		return nullptr;
	}

	FItemData* ItemRow = ItemDataHandle.DataTable->FindRow<FItemData>(ItemDataHandle.RowName, TEXT("GetItemData"));
	if (!ItemRow)
	{
		return nullptr;
	}

	return ItemRow;
}

FName UItemDataComponent::GetRowName()
{
	return ItemDataHandle.RowName;
}

void UItemDataComponent::PickUp(AActor* Interactor)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	if (!Interactor)
	{
		return;
	}

	UInventoryComponent* InvnetoryComp = Interactor->FindComponentByClass<UInventoryComponent>();
	if (!InvnetoryComp)
	{
		return;
	}

	FItemData* ItemRow = GetItemData();
	if (!ItemRow)
	{
		return;
	}

	// 인벤토리에 추가
	if (InvnetoryComp->PickUp(ItemRow->ItemName, ItemQuantity))
	{
		Owner->Destroy();
	}
	
}
