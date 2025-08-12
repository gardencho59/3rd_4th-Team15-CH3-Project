#include "Inventory/Component/ItemDataComponent.h"
#include "Inventory/Data/ItemSlot.h"

UItemDataComponent::UItemDataComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FItemSlot* UItemDataComponent::GetItemData()
{
	if (!ItemDataHandle.DataTable)
	{
		return nullptr;
	}

	return ItemDataHandle.DataTable->FindRow<FItemSlot>(ItemDataHandle.RowName, TEXT("GetItemData"));
}

FName UItemDataComponent::GetRowName()
{
	return ItemDataHandle.RowName;
}

void UItemDataComponent::PickUp()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("Pick Up!"));
	Owner->Destroy();
}
